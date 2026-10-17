# Echoes game text

Native text uses two-byte characters. In this mod use `char16_t`, `u"..."`, and `rstl::wstring`, not host
`wchar_t` or `L"..."`. `rstl::string` and `wstring` occupy 0x10 bytes, including the allocator slot.
Owning constructors take a non-null character pointer and an optional maximum character count
(`-1` requires a terminator and copies through it); copies and destruction use Echoes' reference-counted allocation routines.
`string_l` and `wstring_l` borrow their input: storage must survive every copy, including copies kept by the game.
Lengths count characters, excluding the terminator. Copy assignment is deliberately unavailable.

Native `CTextParser` markup applies to the game's text panes, not ImGui. The version label uses plain text.
The following notes come from Echoes `src/Kyoto/Text/CTextParser.cpp` at decomp revision `ddb9874` and its
G2ME01 disassembly; they are not a promise of tested appearance in every font or pane.

| Syntax | Purpose |
| --- | --- |
| `&&` | Literal ampersand |
| `&font=XXXXXXXX;` | FONT asset ID in hexadecimal |
| `&main-color=#RRGGBB;` | Fill color; optional trailing AA supplies alpha |
| `&fg-color=#RRGGBBAA;` | Foreground color |
| `&outline-color=#RRGGBB;` | Outline color, when supported by the font |
| `&push;` / `&pop;` | Save / restore render state |
| `&line-spacing=N;` | Percentage line spacing, with 100 meaning normal |
| `&line-extra-space=N;` | Additional integer line spacing |
| `&character-extra-space=N;` | Additional integer character spacing |
| `&just=left;`, `center`, `right`, `full`, `nleft`, `ncenter`, `nright` | Horizontal justification |
| `&vjust=top;`, `center`, `bottom`, `full`, `ntop`, `ncenter`, `nbottom` | Vertical justification |
| `&image=XXXXXXXX;` | TXTR asset ID; native images can also use animated/scaled forms |

Tags end at the next semicolon; unknown tags go to the empty `HandleUserTag` implementation.
Use well-formed tags: the parser is permissive and does not generally validate input lengths or numbers.
Avoid `geometry-color`: Echoes' parser advances 11 characters despite the 15-character prefix.
Avoid the fixed-offset `color` override syntax without a separate bounds/retail audit.

Relevant G2ME01 functions: `ParseTag` 0x802B963C, `ParseText` 0x802BA530, `ParseColor` 0x802B9368.
Keep layout-dependent changes subject to an in-game fit check; replacing text does not resize its pane.
