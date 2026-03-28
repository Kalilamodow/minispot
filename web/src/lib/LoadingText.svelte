<script lang="ts">
    import { onMount } from "svelte";

    let dots = $state(1);

    onMount(() => {
        const intervId = setInterval(() => {
            dots++;
            if (dots > 3) dots = 1;
        }, 500);

        return () => clearInterval(intervId);
    });
</script>

<span style="width:10ch">Loading{".".repeat(dots)}</span>
