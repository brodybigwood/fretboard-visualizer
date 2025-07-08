
function midiToJSON(midi, fileName, songName) {
    if (!songName) {
        songName = fileName.replace(/\.[^/.]+$/, "");
    }

    const instrumentMap = new Map();
    let id = 0;

    midi.tracks.forEach(track => {
        track.notes.forEach(note => {
            const channel = note.channel;
            const name = `channel_${channel}`;
            if (!instrumentMap.has(name)) instrumentMap.set(name, []);
            const events = instrumentMap.get(name);

            events.push({
                type: 1,
                note: note.midi,
                velocity: Math.round(note.velocity * 127),
                        time: note.time,
                        channel,
                        id
            });
            events.push({
                type: 0,
                note: note.midi,
                velocity: 0,
                time: note.time + note.duration,
                channel,
                id: id++
            });
        });
    });

    const instruments = Array.from(instrumentMap.entries()).map(([name, events]) => ({
        name,
        events
    }));

    const json = {
        name: songName,
        file: fileName,
        instruments
    };

    return json;
}


const dropzone = document.getElementById('dropzone');

dropzone.addEventListener('dragover', e => {
    e.preventDefault();
    dropzone.style.borderColor = 'green';
});

dropzone.addEventListener('dragleave', e => {
    dropzone.style.borderColor = '#555';
});

dropzone.addEventListener('drop', async e => {
    e.preventDefault();
    dropzone.style.borderColor = '#555';

    const file = e.dataTransfer.files[0];
    if (!file || (!file.name.endsWith('.mid') && !file.name.endsWith('.midi'))) {
        alert('Drop a valid MIDI file.');
        return;
    }

    const arrayBuffer = await file.arrayBuffer();
    const midi = new Midi(arrayBuffer);
    const jsonString = midiToJSON(midi, file.name);

    setSong(jsonString);
});
