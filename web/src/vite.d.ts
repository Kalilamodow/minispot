interface Window {
    onSpotifyWebPlaybackSDKReady: () => void;
    chrome: {
        webview: {
            postMessage: (message: object) => void;
            addEventListener: (
                eventType: string,
                handler: (event: Event) => void,
            ) => void;
        };
    };
}
