<script lang="ts">
    import { onMount } from "svelte";
    import { fly } from "svelte/transition";
    import LoadingText from "./lib/LoadingText.svelte";
    import { SpotifyWeb } from "./lib/spoapi.ts";
    import Player from "./Player.svelte";
    import { isLoading } from "./state.ts";

    type Props = {
        clientId: string;
        accessToken: string;
    };

    let props: Props = $props();

    const player = new Spotify.Player({
        name: "minispot",
        getOAuthToken: (cb) => cb(props.accessToken),
    });
    const api = new SpotifyWeb(props.accessToken);

    let deviceId = $state<string | null>(null);
    $effect(() => {
        if (deviceId) api.startPlayback(deviceId);
    });

    onMount(() => {
        $isLoading = true;
        player.addListener("ready", ({ device_id }) => {
            deviceId = device_id;
            $isLoading = false;
        });

        player.connect();
    });

    let isPlaylistSelectorOpen = $state(false);

    let saved_getPlaylists: ReturnType<SpotifyWeb["getPlaylists"]> | null =
        null;
    const getPlaylists = () =>
        saved_getPlaylists !== null
            ? saved_getPlaylists
            : (saved_getPlaylists = api.getPlaylists());
</script>

{#if deviceId}
    <Player
        {player}
        openPlaylistSelector={() => (isPlaylistSelectorOpen = true)}
    />

    {#if isPlaylistSelectorOpen}
        <div class="playlist-selector" transition:fly>
            <header>
                <h3>Select playlist</h3>
                <button
                    type="button"
                    onclick={() => (isPlaylistSelectorOpen = false)}
                >
                    Cancel
                </button>
            </header>
            {#await getPlaylists()}
                <LoadingText />
            {:then playlists}
                <div class="playlist-carousel">
                    {#each playlists as pl}
                        <button
                            type="button"
                            class="playlist-btn"
                            onclick={() => {
                                if (!deviceId) return;
                                api.startPlayback(deviceId, pl.uri);
                                isPlaylistSelectorOpen = false;
                                $isLoading = true;
                            }}
                        >
                            <img
                                src={pl.images[0].url}
                                alt="{pl.name} thumbnail"
                            />
                            <div>{pl.name}</div>
                        </button>
                    {/each}
                </div>
            {/await}
        </div>
    {/if}
{/if}

<style>
    .playlist-selector {
        position: fixed;
        inset: 0;
        background-color: #222e;
        padding: 8px;

        header {
            margin-bottom: 8px;

            > * {
                display: inline-block;
            }

            > h3 {
                margin: 8px;
            }

            > button {
                float: right;
            }
        }

        .playlist-carousel {
            display: flex;
            flex-direction: row;
            width: 100%;
            overflow-y: hidden;
            overflow-x: auto;
            gap: 8px;

            .playlist-btn {
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
                height: 96px;
                padding: 8px;
                gap: 4px;

                img {
                    max-width: 48px;
                    max-height: 48px;
                    border-radius: 2px;
                }
            }
        }
    }
</style>
