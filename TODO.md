# XOpenPlayer TODO

## Current Status

XOpenPlayer currently has basic X11 window management and simple UI controls. The focus is on building a robust UI using pure X11 before adding audio capabilities.

## Desirable Features

- [ ] Enhance metadata support (album art, artist info).
- [ ] Add gesture control for playback.
- [ ] Support live streaming
- [ ] Implement audio effects (reverb, chorus)
- [ ] Allow customizable skins and layouts
- [ ] Add content filtering options by genre, date, ratings
- [ ] Support multi-channel audio setups
- [ ] Include batch processing for renaming/tagging files.
- [ ] Integrate with music streaming services (e.g., Spotify)
- [ ] Built-in audio equalizer for enhanced sound control
- [ ] Easy creation and management of playlists, including drag-and-drop functionality
- [ ] Remember last playback position for all media
- [ ] Skinnable, or themes to personalize the look and feel
- [ ] Options for enhancing video quality (e.g., de-interlacing, sharpening)
- [ ] Options for loading, editing, and styling subtitles
- [ ] Ability to bookmark specific moments in a video or audio track
- [ ] A feature to view recently played media 
- [ ] Support track list // playlist files .m3u, .pls, .wpl, .asx, .cue
- [ ] Support for .mp3, .mp4, .wav, .ogg, .wma, 
### High Priority

- [ ] Implement proper window resize handling
  - [ ] Make UI elements scale with window size
  - [ ] Save/restore window dimensions between sessions
  - [ ] Add minimum window size constraints

- [ ] Create proper file dialog
  - [ ] Basic file browser using X11 windows
  - [ ] Directory navigation
  - [ ] File filtering for supported formats
  - [ ] Recent files list

- [ ] Enhance menu system
  - [ ] Add dropdown menus for File/Edit/Help
  - [ ] Implement menu keyboard navigation
  - [ ] Add keyboard shortcuts (with visual indicators)
  - [ ] Add right-click context menu

- [ ] Improve playback controls
  - [ ] Add proper button hover states
  - [ ] Create pressed button states
  - [ ] Add tooltips for controls
  - [ ] Implement smooth progress bar updates
  - [ ] Add time position display (00:00 / 00:00)

### Medium Priority

- [ ] Add playlist panel
  - [ ] Implement resizable side panel
  - [ ] Add column headers (Title, Duration, etc.)
  - [ ] Support item selection
  - [ ] Enable drag-and-drop reordering
  - [ ] Add scroll bars for long lists

- [ ] Create volume control widget
  - [ ] Design proper volume slider
  - [ ] Add mute toggle button
  - [ ] Show numeric volume percentage
  - [ ] Add volume icon that changes with level

- [ ] Implement status bar
  - [ ] Show current playback status
  - [ ] Display file format info
  - [ ] Show play time
  - [ ] Add status messages

### Lower Priority

- [ ] Add themes support
  - [ ] Create theme configuration structure
  - [ ] Support custom colors
  - [ ] Add alternative button styles
  - [ ] Support custom fonts

- [ ] Create settings dialog
  - [ ] Window preferences
  - [ ] UI customization options
  - [ ] Keyboard shortcut configuration
  - [ ] Theme selection

- [ ] Add help system
  - [ ] Create about dialog
  - [ ] Add keyboard shortcuts overview
  - [ ] Implement tooltip system
  - [ ] Add context-sensitive help

## Technical UI Tasks

    ### High Priority

    - [ ] Implement double buffering
    - [ ] Reduce window flicker
    - [ ] Smooth UI updates
    - [ ] Proper expose event handling

    - [ ] Add event queue system
    - [ ] Handle rapid UI events properly
    - [ ] Implement event debouncing
    - [ ] Add custom event types

    - [ ] Improve window management
    - [ ] Proper focus handling
    - [ ] Window minimization support
    - [ ] Multiple window support for dialogs

    ### Medium Priority

    - [ ] Create UI widget system
    - [ ] Button widget base class
    - [ ] Slider widget base class
    - [ ] Text input widget
    - [ ] Scrollbar widget

    - [ ] Implement layout management
    - [ ] Add grid layout system
    - [ ] Support dynamic resizing
    - [ ] Add padding and margins
    - [ ] Implement UI constraints

    ### Lower Priority

    - [ ] Add acceleration structures
    - [ ] Implement dirty rectangles
    - [ ] Add UI region clipping
    - [ ] Optimize redraw logic

## Documentation Tasks

- [ ] Document UI component hierarchy
- [ ] Create style guide for UI elements
- [ ] Add developer guidelines for UI contributions
- [ ] Document window management system
- [ ] Create UI testing guidelines

## Code Quality

- [ ] Add UI-specific unit tests
- [ ] Implement UI error handling
- [ ] Add debug visualization modes
- [ ] Create UI performance benchmarks

## Future Considerations

- Keep track of features that will need modification when audio support is added
- Plan UI elements for future audio visualization
- Consider accessibility features
- Plan for internationalization support

## Notes

- All improvements should maintain focus on pure X11 implementation
- Avoid external dependencies for now
- Keep performance in mind, especially for resize operations
- Maintain clean separation between UI and future audio components

## Contributing

When contributing to UI improvements:
1. Focus on one component at a time
2. Maintain consistent styling
3. Test thoroughly on different window sizes
4. Document any new UI components
5. Follow existing code style for X11 calls