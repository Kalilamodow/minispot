export class SpotifyWeb {
    private accessToken: string;

    constructor(accessToken: string) {
        this.accessToken = accessToken;
    }

    public async startPlayback(deviceId: string, uri?: string) {
        console.log(this.accessToken);
        await fetch(
            `https://api.spotify.com/v1/me/player/play?device_id=${deviceId}`,
            {
                method: "PUT",
                headers: {
                    Authorization: `Bearer ${this.accessToken}`,
                    "Content-Type": "application/json",
                },
                body: JSON.stringify(uri ? { context_uri: uri } : {}),
            },
        );
    }

    public async getPlaylists() {
        const response = await fetch(
            "https://api.spotify.com/v1/me/playlists",
            {
                headers: {
                    Authorization: `Bearer ${this.accessToken}`,
                },
            },
        );
        const json = await response.json();
        // mini definition for SimplifiedPlaylistObject
        return json.items as {
            uri: string;
            name: string;
            images: { url: string }[];
        }[];
    }
}
