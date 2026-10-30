# Native Qt regression and screenshot harness

Apply the source patch to the documented upstream baseline. Copy
welcome-tests.pro to the client checkout root, this tests directory below it,
and welcome-example beside it. From that checkout root, build and run:

    qmake welcome-tests.pro CONFIG+=release
    mingw32-make
    release\welcome-tests.exe -o test-results.txt,txt

On a non-MinGW Qt environment, use its corresponding make command and executable
path. Only the Windows Qt 5.5.1 build under Wine has been exercised for this
extraction. Use a dedicated development environment.

The harness uses the real LoginWindow and WelcomeScreen classes with a separate
LibkiWelcomeTests/IsolatedDemo settings identity. It clears that test identity
on startup. It does not instantiate NetworkClient or execute production main().
The login test verifies credential signal emission without sending credentials
to a server. The screenshot test uses empty fields and generic sample content.

The test executable needs the same Qt runtime/plugin dependencies as the client,
plus Qt Test. Run from the checkout root so welcome-example is found.
