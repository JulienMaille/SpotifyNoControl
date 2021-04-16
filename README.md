# SpotifyNoControl
Realated to https://github.com/morpheusthewhite/spicetify-themes/tree/master/DribbblishDynamic

You can use `SpotifyNoControl.exe`, included in this theme package, to completely remove all windows controls and title menu (three dot at top left corner). Title menu still can be access via Alt key. Closing, minimizing can be done via right click menu at top window region.  
`SpotifyNoControl.exe` could be used as Spotify launcher, it opens Spotify and hides controls right after. You can drag and drop it to your taskbar but make sure you unpin the original Spotify icon first. Alternatively you can make a shortcut for it and add to desktop or start menu.  
Moreover, by default, Spotify adjusted sidebar items and profile menu icon to stay out of Windows native controls region. If you decided to use `SpotifyNoControl.exe` from now on, please open `user.css` file and change variable `--os-windows-icon-dodge` value to 0 as instruction to snap icons back to their original position.

```
USAGE: SpotifyNoControl.exe [SpotifyNoControl.lnk] [Spotify.lnk] [spotify.exe]
  - [SpotifyNoControl.lnk] is the path to your shortcut pointing to SpotifyNoControl
    defaults to %appdata%\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\SpotifyNoControl.lnk
             or %appdata%\Microsoft\Windows\Start Menu\Programs\SpotifyNoControl.lnk
  - [Spotify.lnk] is the path to your shortcut pointing to Spotify
    defaults to %appdata%\Microsoft\Windows\Start Menu\Programs\Spotify.lnk
  - [spotify.exe] is the path to Spotify executable
    defaults to %appdata%\Spotify\spotify.exe
```
