import random
import time

from Wildcat.Graphics.Overlay.Overlay import TextOverlay

WORDS = [
    "hello", "world", "overlay", "stream", "delta", "colour", "random",
    "python", "window", "typing", "effect", "characters", "minute",
    "wrap", "boundary", "threfffffffffffad", "queue", "buffer", "render",
    "transparent", "shasssssssdow", "paint", "flush", "instant", "feed",
    "lorem", "ipsum", "dolor", "sit", "amet", "AAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBAAAAAAAAAAAAAAAAAA"
]


def random_rgb():
    # avoid very dark colors that are hard to read
    return (
        random.randint(60, 255),
        random.randint(60, 255),
        random.randint(60, 255),
    )


def main():
    cpm = 4000  # characters per minute (5 chars/sec). Adjust to taste.

    overlay = TextOverlay(
        x=None,
        y=40,
        w=680,
        h=420,
        max_lines=18,
        wrap_cols=44,
        cpm=cpm,
    )

    # Use instant only once at startup (fine)
    overlay.instant(f"Typewriter test (CPM={cpm})\n\n", (200, 200, 255))

    try:
        while True:
            word = random.choice(WORDS)
            color = random_rgb()

            # IMPORTANT: only feed() during the loop, so pacing is respected
            overlay.feed(word, color)
            overlay.feed(" ", color)

            # This sleep controls how fast you enqueue words, not how fast they render.
            # You can enqueue quickly; the overlay will still type at CPM.
            time.sleep(0.00001)

    except KeyboardInterrupt:
        overlay.close()
        print("Stopped.")


if __name__ == "__main__":
    main()
