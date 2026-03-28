<script lang="ts">
    import App from "./App.svelte";
    import Credentials from "./Credentials.svelte";
    import LoadingText from "./lib/LoadingText.svelte";
    import { isLoading } from "./state.ts";

    const params = new URLSearchParams(location.search);

    let clientId = $state(params.get("client_id"));
    let accessToken = $state(params.get("access_token"));
    let spotifyReady = $state(false);

    window.onSpotifyWebPlaybackSDKReady = () => {
        spotifyReady = true;
        // !clientId && accessToken -> impossible -> debug backend
        // !clientId && !accessToken -> no credentials -> show ui
        // clientId && accessToken -> good to go -> show ui
        // clientId && !accessToken -> unauthorized -> get auth code
        if (clientId && !accessToken) {
            const scope =
                "streaming user-read-email user-read-private playlist-read-private user-modify-playback-state";

            const auth_query_parameters = new URLSearchParams({
                response_type: "code",
                client_id: clientId,
                scope: scope,
                redirect_uri: "http://127.0.0.1:20956/",
            });

            window.location.href =
                "https://accounts.spotify.com/authorize/?" +
                auth_query_parameters.toString();
        } else {
            $isLoading = false;
        }
    };
</script>

{#if !clientId}
    <Credentials />
{:else if accessToken && spotifyReady}
    <App {clientId} {accessToken} />
{/if}
{#if $isLoading}
    <div class="fs-loading">
        <LoadingText />
    </div>
{/if}

<style>
    .fs-loading {
        position: fixed;
        inset: 0;
        background-color: #000a;

        display: flex;
        align-items: center;
        justify-content: center;

        font-size: 24px;
        font-weight: bold;
    }
</style>
