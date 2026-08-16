# Final Verification Record

**Student:** Parminder Singh  
**Course:** CS 499: Computer Science Capstone  
**Verification date:** August 15, 2026

## Packaged artifact checks

The commands below were run against the files inside this final project folder.

| Category | Verification | Result |
|---|---|---|
| Software design and engineering | C++17 build with `-Wall -Wextra -Wpedantic -Werror` | Passed |
| Software design and engineering | Runtime verification suite | 13 passed, 0 failed |
| Software design and engineering | AddressSanitizer and UndefinedBehaviorSanitizer | Passed |
| Algorithms and data structures | Reusable implementation + separate demonstration build | Passed |
| Algorithms and data structures | Separate verification suite | 19 passed, 0 failed |
| Algorithms and data structures | Large-input performance comparison | Completed; both implementations returned matching results |
| Algorithms and data structures | AddressSanitizer and UndefinedBehaviorSanitizer | Passed |
| Databases | Python unit-test suite | 6 passed, 0 failed |

## Performance evidence

The performance comparison uses 5,000 type names with 250 searches for the last item and an inclusive range from 0 through 50,000,000. One recorded representative run showed approximately 1,602 times faster membership lookup and approximately 3.32 million times faster range summation. A final packaging run showed approximately 1,534 times and 3.30 million times respectively. Timing varies by computer; both runs demonstrate the same difference in algorithmic growth, and both implementations were checked for matching results before timing was reported.

## Portfolio checks

- All five HTML pages contain a document title, one primary heading, primary navigation, and a main-content region.
- Every relative link and media source in the HTML resolves to a packaged file.
- The professional self-assessment appears first on `index.html` and is readable inline.
- Each artifact page contains its full narrative and direct links to the original code, enhanced code, verification evidence, and Word narrative.
- The code-review page embeds the MP4 and provides direct download links to the video, script, and original artifact.
- All five Word documents were rendered and visually reviewed page by page for clipping, overlap, broken tables, and placeholder content.
- Generated caches, build binaries, runtime databases, and operating-system metadata were excluded from the final package.

## Final external step

The project is GitHub Pages-ready. Publishing requires uploading the folder contents to the connected GitHub repository, enabling Pages from the repository root, and copying the resulting public URL into the course submission.
