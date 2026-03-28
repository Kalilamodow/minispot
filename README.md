# minispot

Small, self-contained WebView2 Spotify player.

## getting credentials

When you first open the app, a screen will pop up asking for your client ID and client secret. For these, you'll need a Spotify development app. To make one, go to [the Spotify Developer Dashboard](https://developer.spotify.com/dashboard) and make a new app. Make sure one redirect URL is `http://127.0.0.1:20956/` (including the ending slash!) and the enabled APIs include "Web Playback" and "Web API". Save it and go to the top where your Client ID and Client Secret are, then you can just paste them into minispot.
