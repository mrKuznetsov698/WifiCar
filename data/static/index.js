let speedComp;
let sliderComp;
let buttons = [];
const schema = ["leftup", "forw", "rightup", "left", "right", "leftdown", "back", "rightdown"];
let flag = true;

function init() {
    speedComp = document.getElementById('speed_txt');
    sliderComp = document.getElementById('speed_sl');
    
    sliderComp.oninput = slider_updated;
    sliderComp.onchange = (ev) => {
        make_request(`/api/speed`, {spd: sliderComp.value});
    };
    document.onkeyup = keyUp;
    document.onkeydown = keyDown;
    document.querySelectorAll('.container>.table>.cell>.button>i').forEach(function(i) {
        buttons.push(i.parentElement);
    })
    for (let i = 0; i < 8; i++) {
        buttons[i].ontouchstart = buttons[i].onmousedown = (ev) => {clicked(i)};
        buttons[i].ontouchend = buttons[i].onmouseup = (ev) => {released(i)};
    }
}

function slider_updated(ev) {
    let val = sliderComp.value;
    let str = `Speed: ${val}`
    speedComp.textContent = str;
    // make_request(`/api/speed`, {spd: val})
}

function clicked(num) {
    if (num < 0 || num > 7) {
        return;
    }
    buttons[num].classList.add('active');
    make_request('/api/run', {act: schema[num]});
    console.log('clicked', num);
}

function released(num) {
    console.log('released', num);
    buttons[num].classList.remove('active');
    make_request('/api/stop', {});
}

function keyUp(ev) {
    if (flag) {
        return;
    }
    flag = true;
    if (ev.code == 'Numpad8') {
        released(1);
    } else if (ev.code == 'Numpad2') {
        released(6);
    } else if (ev.code == 'Numpad4') {
        released(3);
    } else if (ev.code == 'Numpad6') {
        released(4);
    }
}

function keyDown(ev) {
    if (!flag) {
        return;
    }
    flag = false;
    if (ev.code == 'Numpad8') {
        clicked(1);
    } else if (ev.code == 'Numpad2') {
        clicked(6);
    } else if (ev.code == 'Numpad4') {
        clicked(3);
    } else if (ev.code == 'Numpad6') {
        clicked(4);
    } else {
        flag = true;
    }
}

function make_request(url, params) {
    fetch(url + '?' + new URLSearchParams(params))
    .then((response) => console.log(response))
    .catch((error) => console.error(error));
}