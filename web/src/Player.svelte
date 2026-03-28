<script lang="ts">
    import { onMount } from "svelte";
    import Icon from "./lib/Icon.svelte";
    import { isLoading } from "./state";

    type Props = {
        player: Spotify.Player;
        openPlaylistSelector: () => void;
    };

    let { player, openPlaylistSelector }: Props = $props();

    let currentTrack = $state<Spotify.Track | null>(null);
    let playing = $state(false);

    $effect(() => {
        if (currentTrack) $isLoading = false;
    });

    const playerStateChanged = (state: Spotify.PlaybackState) => {
        if (!state) return;
        currentTrack = state.track_window.current_track;
        playing = !state.paused;
    };

    onMount(() => {
        $isLoading = true;
        player.addListener("player_state_changed", playerStateChanged);
        return () =>
            player.removeListener("player_state_changed", playerStateChanged);
    });
</script>

<div id="player">
    {#if currentTrack}
        <img
            src={currentTrack.album.images[0].url}
            alt="Album cover for {currentTrack.name}"
        />
        <div id="player-ui">
            <div id="song-details">
                <header>{currentTrack.name}</header>
                <div id="song-artists">
                    {currentTrack.artists.map((x) => x.name).join(", ")}
                </div>
            </div>
            <div id="controls">
                <button onclick={() => player.previousTrack()}
                    ><Icon icon="prev" /></button
                >
                <button onclick={() => player.togglePlay()}>
                    <Icon icon={playing ? "playing" : "paused"} />
                </button>
                <button onclick={() => player.nextTrack()}
                    ><Icon icon="next" /></button
                >
            </div>
            <br />
            <button type="button" onclick={() => openPlaylistSelector()}>
                Change playlist
            </button>
        </div>
    {/if}
</div>

<style>
    #player {
        margin-top: -5px; /* body padding */
        height: 100vh;
        display: flex;
        flex-direction: row;
        align-items: center;
        gap: 32px;
        padding: 0 16px;

        div#player-ui {
            height: 128px;
            flex: 1;
            display: flex;
            flex-direction: column;
            justify-content: center;

            header {
                font-weight: bold;
                margin-top: 8px;
            }

            div#song-details {
                flex: 1;
                display: flex;
                flex-direction: column;
                justify-content: center;

                div#song-artists {
                    color: #ccc;
                }
            }

            div#controls {
                margin-top: auto;
            }
        }

        > img {
            height: 128px;
            border-radius: 4px;
        }
    }
</style>
