
let player;
let songTime = 0;
let timePtr;
let timeHeap;
let playerReady = false;

function extractVideoId(url) {
    const match = url.match(/(?:v=|\/)([0-9A-Za-z_-]{11})/);
    return match ? match[1] : null;
}

function loadVideo() {
    const url = document.getElementById('ytUrl').value;
    const id = extractVideoId(url);
    if (!id) return alert("invalid url");

    if (playerReady) {
    player.loadVideoById(id);
    } else {
    player = new YT.Player('player', {
        videoId: id,
        playerVars: {
        controls: 0,
        modestbranding: 1,
        rel: 0,
        enablejsapi: 1,
        disablekb: 1,
        },
        events: {
        'onReady': () => {
            playerReady = true;
            console.log('player ready');
            requestAnimationFrame(loop);
        }
        }
    });
    }
}

function playVideo() {
    if (playerReady) player.playVideo();
}

function pauseVideo() {
    if (playerReady) player.pauseVideo();
}

function seek(seconds) {
    if (playerReady) {
    const time = Math.max(0, player.getCurrentTime() + seconds);
    player.seekTo(time, true);
    }
}

function restartVideo() {
    if (playerReady) {
    player.seekTo(0, true);
    player.playVideo();
    }
}

function loop() {
    if (playerReady && typeof player.getPlayerState === 'function' && player.getPlayerState() === YT.PlayerState.PLAYING) {
    songTime = player.getCurrentTime();
    if (timeHeap) timeHeap[0] = songTime;
    }
    requestAnimationFrame(loop);
}


function handleNotesInput() {
    const input = document.getElementById('notes').value;
    const nums = input.split(',').map(x => parseFloat(x.trim())).filter(x => !isNaN(x));
    const nBytes = nums.length * 4;
    const ptr = Module._malloc(nBytes);
    Module.HEAPF32.set(nums, ptr >> 2);
    Module.ccall('updateNotes', null, ['number', 'number'], [ptr, nums.length]);
    Module._free(ptr);
}

function loadSong() {
    const link = document.getElementById('link').value;
    fetch(link)
    .then(res => res.json())
    .then(obj => {
        const str = JSON.stringify(obj);
        const len = lengthBytesUTF8(str) + 1;
        const ptr = Module._malloc(len);
        stringToUTF8(str, ptr, len);
        Module.ccall('recieveJSON', null, ['number'], [ptr]);
        Module._free(ptr);
    });
}

var Module = {
    canvas: document.getElementById('canvas'),
    onRuntimeInitialized() {
    console.log('WASM app ready');
    timePtr = Module.ccall('getTimePtr', 'number', [], []);
    timeHeap = new Float32Array(Module.HEAPF32.buffer, timePtr, 1);
    }
};
