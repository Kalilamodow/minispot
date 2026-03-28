<script lang="ts">
    import { isLoading } from "./state.ts";

    let clientIdTxt = $state("");
    let clientSecTxt = $state("");

    const done = () => {
        $isLoading = true;
        window.chrome.webview.postMessage({
            message_id: 0,
            client_id: clientIdTxt,
            client_secret: clientSecTxt,
        });
    };
</script>

<div class="credentials">
    <h3>Spotify credentials</h3>

    <div class="input-group">
        <label for="client-id-txt">Client ID</label>
        <input
            type="text"
            placeholder="Enter client ID..."
            id="client-id-txt"
            bind:value={clientIdTxt}
        />
    </div>

    <div class="input-group">
        <label for="client-sec-text">Client Secret</label>
        <input
            type="text"
            placeholder="Enter client secret..."
            id="client-sec-txt"
            bind:value={clientSecTxt}
        />
    </div>

    <button type="button" onclick={done}>Done</button>
</div>

<style>
    div.credentials {
        position: fixed;
        inset: 8px;

        display: flex;
        flex-direction: column;
    }

    h3 {
        margin: 0;
        margin-bottom: 8px;
    }

    div.input-group {
        display: flex;
        flex-direction: row;
        align-items: center;
        justify-content: space-between;
        margin-bottom: 8px;
    }

    input {
        width: 24ch;
    }

    button {
        margin-top: auto;
    }
</style>
