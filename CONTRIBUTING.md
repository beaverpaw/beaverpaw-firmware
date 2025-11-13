# Contributing to BeaverPaw Firmware

Thank you for considering contributing to BeaverPaw! 🎉

## How You Can Help

### For Everyone
- **Report bugs** - Found something that doesn't work? Let us know!
- **Suggest features** - Have an idea? We'd love to hear it
- **Improve documentation** - Help us make guides clearer
- **Share your projects** - Show us what you've built!

### For Developers
- Fix bugs
- Add new features
- Optimize code
- Write tests

## Reporting Bugs

Found a problem? Here's how to report it:

1. **Check if it's already reported** in [Issues](https://github.com/beaverpaw/beaverpaw-firmware/issues)
2. **Create a new issue** with:
   - What you expected to happen
   - What actually happened
   - Steps to reproduce the problem
   - Your device model (ESP32 Generic or Security Cube)
   - Firmware version you're using
   - Any error messages from Serial Monitor

## Suggesting Features

Have an idea? We want to hear it!

1. Open an issue with tag "enhancement"
2. Describe what you want to do
3. Explain why it would be useful
4. If you have implementation ideas, share them!

## Contributing Code

### Getting Started

1. **Fork this repository** (click Fork button on GitHub)
2. **Clone your fork** to your computer
3. **Create a branch** for your feature: `git checkout -b my-cool-feature`
4. **Make your changes**
5. **Test on real hardware** (very important!)
6. **Commit your changes**: `git commit -m "Add cool feature"`
7. **Push to GitHub**: `git push origin my-cool-feature`
8. **Open a Pull Request** on our repository

### Code Style

- Use clear variable names
- Add comments for complex logic
- Keep functions small and focused
- Test on actual hardware before submitting

### Testing Requirements

Before submitting code, please test:
- ✅ WiFi AP setup works (test on iPhone if possible)
- ✅ Bluetooth setup works (test on Android)
- ✅ Device connects to AWS IoT
- ✅ Commands work correctly
- ✅ No crashes or errors in Serial Monitor

## Security Vulnerabilities

**Important:** If you find a security issue, please **DO NOT** open a public issue.

Email us privately at: **security@beaverpaw.com**

We'll work with you to fix it quickly and credit you in the release notes.

## Code of Conduct

Be nice! We're all here to learn and build cool things together.

- Be respectful and welcoming
- Be patient with newcomers
- Give constructive feedback
- Focus on what's best for the community

## Questions?

Not sure about something? Just ask!

- 💬 [GitHub Discussions](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
- 📧 Email: support@beaverpaw.com

## License

By contributing, you agree that your contributions will be licensed under the Apache 2.0 License.

---

Thank you for helping make BeaverPaw better! 🐕
