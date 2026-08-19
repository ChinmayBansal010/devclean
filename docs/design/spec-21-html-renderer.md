# HTML Renderer

HTML reports are static representations of normalized results.

The renderer must escape cache names, paths, and diagnostics before insertion into markup. It should not embed executable scripts or environment secrets. Styling is presentation-only and must not affect report meaning.