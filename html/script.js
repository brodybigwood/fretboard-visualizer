
let player;
let songTime = 0;
let timePtr;
let timeHeap;
let playerReady = false;

function extractVideoId(url) {
    const match = url.match(/(?:v=|\/)([0-9A-Za-z_-]{11})/);
    return match ? match[1] : null;
}

function loadVideo(id=null) {
    if(!id) {
        const url = document.getElementById('ytUrl').value;
        id = extractVideoId(url);
    }
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
        },
        'onStateChange': onPlayerStateChange
        }
    });
    }
}

function togglePlayPause() {
    if (!playerReady) return;

    const icon = document.getElementById('playPauseIcon');
    const state = player.getPlayerState();

    if (state === YT.PlayerState.PLAYING) {
        player.pauseVideo();
        icon.textContent = 'play_arrow';
    } else {
        player.playVideo();
        icon.textContent = 'pause';
    }
}

function onPlayerStateChange(event) {
    const icon = document.getElementById('playPauseIcon');
    if (event.data === YT.PlayerState.PLAYING) icon.textContent = 'pause';
    else icon.textContent = 'play_arrow';
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

var offset = 0;

function getOffset() {         //how early to show notes in realtime
    return offset + (parseFloat(document.getElementById('offset').value) || 0);
}

function loop() {
    if (playerReady && typeof player.getPlayerState === 'function' && player.getPlayerState() === YT.PlayerState.PLAYING) {
        songTime = player.getCurrentTime() + getOffset()*player.getPlaybackRate();
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
        setSong(obj);
    });
}

function sanitizeSong(obj) {
    if (typeof obj !== 'object' || obj === null) {
        console.warn('Invalid song object:', obj);
        return {};
    }

    const safeObj = {};

    safeObj.name = typeof obj.name === 'string' ? obj.name : '';
    if (safeObj.name === '') console.warn('Missing or invalid song name');

    safeObj.artist = typeof obj.artist === 'string' ? obj.artist : '';
    if (safeObj.artist === '') console.warn('Missing or invalid artist');

    safeObj.link = typeof obj.link === 'string' ? obj.link : '';
    if (safeObj.link === '') console.warn('Missing or invalid song link');

    safeObj.offset = typeof obj.offset === 'number' ? obj.offset : 0;
    if (typeof safeObj.offset !== 'number') console.warn('Missing or invalid offset');

    safeObj.instruments = Array.isArray(obj.instruments)
    ? obj.instruments.map((inst, i) => {
        const safeInst = {};
        safeInst.name = typeof inst.name === 'string' ? inst.name : '';
        if (safeInst.name === '') console.warn(`Instrument ${i} missing or invalid name`);

        safeInst.events = Array.isArray(inst.events)
        ? inst.events.map((ev, j) => {
            const safeEv = {
                type: Number.isInteger(ev.type) ? ev.type : 0,
                          note: typeof ev.note === 'number' ? ev.note : 0.0,
                          velocity: Number.isInteger(ev.velocity) ? ev.velocity : 0,
                          time: typeof ev.time === 'number' ? ev.time : 0.0,
                          id: Number.isInteger(ev.id) ? ev.id : -1,
            };
            if (!Number.isInteger(ev.type)) console.warn(`Event ${j} in instrument ${i} missing or invalid type`);
            if (typeof ev.note !== 'number') console.warn(`Event ${j} in instrument ${i} missing or invalid note`);
            if (!Number.isInteger(ev.velocity)) console.warn(`Event ${j} in instrument ${i} missing or invalid velocity`);
            if (typeof ev.time !== 'number') console.warn(`Event ${j} in instrument ${i} missing or invalid time`);
            if (!Number.isInteger(ev.id)) console.warn(`Event ${j} in instrument ${i} missing or invalid id`);
            return safeEv;
        })
        : [];
        if (!Array.isArray(inst.events)) console.warn(`Instrument ${i} has no valid events array`);
        return safeInst;
    })
    : [];
    if (!Array.isArray(obj.instruments)) console.warn('Song missing or invalid instruments array');

    return safeObj;
}

let currentSongData = null;

function setSong(obj) {
    const sanitized = sanitizeSong(obj);
    currentSongData = sanitized;
    loadVideo(obj.link);
    offset = obj.offset;
    const str = JSON.stringify(sanitized);
    const len = lengthBytesUTF8(str) + 1;
    const ptr = Module._malloc(len);
    stringToUTF8(str, ptr, len);
    Module.ccall('recieveJSON', null, ['number'], [ptr]);
    Module._free(ptr);
    console.log('Passed sanitized song JSON to WASM');
}


var Module = {
    canvas: document.getElementById('canvas'),
    onRuntimeInitialized() {
    console.log('WASM app ready');
    timePtr = Module.ccall('getTimePtr', 'number', [], []);
    timeHeap = new Float32Array(Module.HEAPF32.buffer, timePtr, 1);
    }
};




const progressContainer = document.getElementById('progressContainer');
const progressBar = document.getElementById('progressBar');

function updateProgressBar() {
    if (playerReady && player && typeof player.getCurrentTime === 'function' && typeof player.getDuration === 'function') {
        const current = player.getCurrentTime();
        const duration = player.getDuration();
        if (duration > 0) {
            const percent = (current / duration) * 100;
            progressBar.style.width = percent + '%';
        }
    }
    requestAnimationFrame(updateProgressBar);
}

progressContainer.addEventListener('click', e => {
    if (!playerReady) return;
    const rect = progressContainer.getBoundingClientRect();
    const clickX = e.clientX - rect.left;
    const newTime = (clickX / rect.width) * player.getDuration();
    player.seekTo(newTime, true);
});

window.addEventListener('keydown', e => {
    if (!playerReady) return;
    switch (e.key) {
        case 'ArrowLeft':
            seek(-5);
            break;
        case 'ArrowRight':
            seek(5);
            break;
        case ' ':
            e.preventDefault();
            if (player.getPlayerState() === YT.PlayerState.PLAYING) pauseVideo();
            else playVideo();
            break;
    }
});

if (playerReady) updateProgressBar();
else {
    const checkReady = setInterval(() => {
        if (playerReady) {
            updateProgressBar();
            clearInterval(checkReady);
        }
    }, 100);
}



const basePath = 'https://brodybigwood.com/file/json/songs/';
const manifestUrl = basePath + 'manifest.json';

function populateSongList() {
    fetch(manifestUrl)
    .then(res => res.json())
    .then(data => {
        const list = document.getElementById('songList');
        const sorted = data.songs.slice().sort((a, b) => a.localeCompare(b));
        for (const file of sorted) {
            fetch(basePath + file)
            .then(res => res.json())
            .then(song => {
                const li = document.createElement('li');
                li.textContent = `${song.name || file} - ${song.artist || 'Unknown'}`;
                li.style.cursor = 'pointer';
                li.style.padding = '4px';
                li.onmouseenter = () => li.style.background = '#333';
                li.onmouseleave = () => li.style.background = 'transparent';
                li.onclick = () => {
                    document.getElementById('link').value = basePath + file;
                    loadSong();
                };
                document.getElementById('songList').appendChild(li);
            });
        }
    });
}

populateSongList();

function downloadCurrentJSON() {
    if (!player || !player.getVideoData) {
        alert("Video must be loaded first.");
        return;
    }
    if (!currentSongData) {
        alert("No song data loaded.");
        return;
    }

    const name = prompt("Enter song name:", currentSongData.name || "Untitled") || "Untitled";
    const artist = prompt("Enter artist name:", currentSongData.artist || "Unknown") || "Unknown";
    const link = player.getVideoData().video_id;

    const jsonToDownload = {
        name,
        artist,
        link,
        offset: getOffset(),
        instruments: currentSongData.instruments || [{ name: "guitar", events: [] }]
    };

    const blob = new Blob([JSON.stringify(jsonToDownload, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${name.replace(/\s+/g, '_')}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}


const speedSlider = document.getElementById('speedSlider');
const speedLabel = document.getElementById('speedLabel');

speedSlider.addEventListener('input', () => {
    const rate = parseFloat(speedSlider.value);
    speedLabel.textContent = rate.toFixed(2) + 'x';
    if (player && player.setPlaybackRate) player.setPlaybackRate(rate);
});
