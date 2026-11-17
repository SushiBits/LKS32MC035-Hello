# PROMPTS.md

How this repository was built with an AI assistant, what the assistant was good
for, where it was wrong, and the rules that came out of that.

This file exists because the project's most expensive lesson was epistemic, not
technical: the vendor's machine-readable artifacts lie, and so does anything that
reasons confidently from them. An assistant is another such artifact. Recording
how it was used, and how often it had to be corrected, is part of the
provenance.

Assistant: GitHub Copilot (Copilot SDK in VS Code). Human: Beizong "Max" Chen.
Period covered: 2026-11-01 to 2026-11-18.

## The division of labour

The assistant never had hardware access, a scope, or the reference manual in a
readable form. It had the source tree, git history, the SVD, and whatever the
human pasted in. Everything it produced was therefore one of four kinds of work:

| Kind | Example | Who judged it |
|---|---|---|
| Reading the tree | "what does this file depend on", "how many lines did that commit delete" | machine-checkable |
| Drafting prose | commit messages, this file, README | human taste |
| Analysing artifacts | SVD field coverage, disabled warnings, dead code paths | human, against hardware |
| Writing code | `systick.c`, `gpio.c`, GPIO/UTIMER merges | compiler, then hardware |

The fourth kind is the one that needed the most care, because code that compiles
is not code that is correct, and this project's failure modes are all silent.

## Prompts that worked

Verbatim, in the order they were used. These are short because the tree was the
context.

**Interrogation before change.**

> This code file seems to have a painful blog post in it. Analyze the pain but
> don't recommend fixes.

The no-fixes constraint produced a better analysis than any request for a fix.
It forced description instead of prescription, and the description is what
surfaced the pattern.

> Check again.

Used four times, always after the human changed something. Consistently produced
more than the first read, because the first read was working from a diff and the
second from the resulting state.

**Supplying the oracle.**

> IC user manual: 如果SYS_CLK_CFG 选择 PLL时钟，则PLLPDN是被硬件控制的…

> IWDG_CFG的写入无需向IWDG_PSW写入密码。

> 写0同时向[15:8] 写入0x3C可清零

Each of these retracted an assistant claim. The pattern is worth stating
plainly: **the assistant was wrong about vendor hardware every time it reasoned
from absence** — no readback path found, therefore unobservable; no password
write present, therefore a missing step; no field in the SVD, therefore
undocumented. The manual said otherwise in one sentence each time. The prompts
that fixed it were not clever; they were citations.

**Refusing to let it invent.**

> Now I am ready for a tacky commit message again.

Commit messages were always drafted separately from changes, never bundled, so
the narrative could be edited without touching code and vice versa.

> Write a commit message for the current edits. (The observability code did not
> work.)

The parenthetical is the important half. Telling the assistant what failed meant
the commit message recorded a negative result instead of describing intent.

**Fixing the register map.**

> I am still making SVD edits to symbolify magic numbers from the RM.

> Validated that UTIMER0 and 1 aren't exactly the same, UTIMER0 is 16-bits and
> UTIMER1 is 32-bits, but other than this width they are otherwise identical. So
> maybe chop off UTIMER0 and keep UTIMER1 structs.

That second prompt overrode the assistant's implicit assumption that identical
structure meant identical parts. The assistant would have merged them the wrong
way round — deriving the wide from the narrow — without it.

**Scope control.**

> Help me with a commit message and the commit removing the vendored CMSIS as I
> am reimporting it as a submodule later.

Followed by a choice: deletions only, or everything. Choosing "deletions only"
kept a build-breaking commit separate from the commit that repaired it, which is
the only reason `git log` reads as a narrative instead of a pile.

## Where the assistant was wrong

Recorded because a confident assistant is the easiest thing in the world to
believe, and this file is a receipt.

| Claim | Correction | Cost if believed |
|---|---|---|
| The generated header should be included explicitly by `system_LKS32MC03x.c` | It is included transitively; CMSIS device headers do that | Style churn in the one file everyone trusts |
| `PLLPDN` should be read back before selecting the PLL | Hardware owns that bit while PLL is selected; there is nothing to read | A "fix" that changes nothing and hides the real constraint |
| `IWDG->PSW` must be written before `IWDG->CFG` | CFG carries its own key in the data; PSW is not involved | An extra write to a register whose protocol was unknown |
| `0x3300` in `AFR[2]` sets functions on P0.9 and P0.10 | It sets P0.10 and P0.11 | A pin map that looks verified and is off by one |
| The vendor J-Link package is corrupt | It is pinned to v6.19 and overwrites `JLinkDevices.xml` wholesale | Wrong diagnosis; the fix is the same but the reason matters |
| "The only record in the world" of a register's behaviour (×3) | Every time, it was in the RM | Three rounds of misplaced importance |

The common thread is one failure mode: **inferring absence from absence of
evidence in the artifacts the assistant could see.** Not a single error was
arithmetic or syntax. All of them were confidence about what a document does not
say.

## Rules that came out of this

1. **The assistant does not state vendor facts it cannot cite.** When the RM
   says otherwise, the RM wins, and the retraction goes in the commit message.
2. **"Verified" means compile and link.** Not "works". Commits that have run on
   hardware say so; commits that have not, say that instead.
3. **Facts and inferences are marked differently, everywhere.** `0x3C` in
   `IWDG_CFG[15:8]` is measured and documented. `LRC2` and the `0P121`/`0P122`
   trim codes are guesses with serial numbers, labelled as guesses in the commit
   that created them.
4. **Negative results are committed.** The `PIE` observation experiment that
   failed is still in `gpio.c`, with a comment saying it failed. Deleting it
   would have erased the only record that the approach was tried.
5. **Silence is data.** The SVD reports `0 errors` while emitting 620 INFO
   messages and 17 warnings. A tool that summarises a wall of noise into a
   friendly number is a tool that hides defects, which is also true of an
   assistant that summarises a repository into a confident paragraph.

## What the assistant was actually for

Not autocomplete. The useful output was:

- **Reading things nobody wanted to read.** 10,900 lines of SVD, 55,214 lines of
  CMSIS, a 1,341-line DMA block. Counting, grouping, diffing, finding the
  duplicated registers and the 13 registers whose reset masks do not fit.
- **Holding the whole history at once.** Which commit deleted the sentence about
  the debugger, which commit put `0x67` in without unlocking first. A human can
  do this with `git log -S`; the assistant does it without being asked.
- **Drafting prose that a human then corrected.** Every commit message in this
  repository was drafted by the assistant and edited or rewritten by the human.
  The tacky voice is joint work; the accuracy is the human's.
- **Refusing to invent.** The best outcome of the protocol above is that when a
  register's meaning was unknown, the assistant said so, and the unknown got
  written down instead of filled in.

## Reproducing this

The setup that made the above work:

- The full tree in context, plus a git history with detailed commit messages. The
  assistant's best answers came from reading the history, not the code.
- Vendor documents pasted verbatim, in the original language, when a factual
  question arose. Summaries were never good enough.
- Build and disassembly as the only automatic verification. `make` and
  `objdump` settled several arguments that discussion could not.
- A human willing to say "check again" after every change, and to correct the
  assistant with a citation rather than an opinion.

## Note on this file

This document was written by the assistant it describes, from the transcript of
its own sessions, at the human's request. That makes it a primary source with an
interested author. Treat it the way the rest of this repository treats the SVD:
useful, specific, and not authoritative without checking.
