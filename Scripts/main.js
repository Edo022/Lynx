

let left = document.querySelector("body > left-");
let right = document.querySelector("body > right-");


// smooth = true|false
function move_to_view(smooth){
    let hash = location.hash.slice(1);
    let id = hash;
    if(hash.length == 0 || hash == null || document.getElementById(id) == null) id = "overview";

    let e = document.getElementById(id); if(!e) return;
    e.scrollIntoView({ block: "start"  , behavior: smooth ? "smooth" : "auto" });
    e = document.getElementById("index--" + id); if(!e) return;
    e.scrollIntoView({ block: "nearest", behavior: smooth ? "smooth" : "auto" });
}





function view(){
    // This scrolls to the correct header, which is the same the browser already scrolled to.
    // Apparently, JS is loaded after scrolling to it and adding new HTML elements messes everything up,
    // so JS has to scroll again after the new elements are loaded.
    // Anything else refuses to work
    move_to_view("smooth");

    // The id="main-mask" div is used to hide the page before js is done moving stuff around as anything else just doesn't work
    // This line removes it from the body so that the user can see the page and think it loaded flawlessly
    let e = document.getElementById('main-mask');
    e.style.pointerEvents = 'none';
    e.style.opacity = '0%';
}



function log_function(f, name){
    var startTime = performance.now()
    f();
    var endTime = performance.now()
    console.log(`${ name } took ${endTime - startTime} ms`)
}



function init_primitive(){
    log_function(ui_slider.init, "ui_slider");
    log_function(setup_index.init, "setup_index");

    view();
}


function init_async() {
    log_function(setup_fix.init, "setup_fix");
    log_function(setup_syntax.init, "setup_syntax");
    log_function(setup_copy_syntax.init, "setup_copy_syntax");

    log_function(summary_list.init, "summary_list");
    log_function(readability.init, "readability");
    log_function(ui_copy_code.init, "ui_copy_code");
    //!^ Must be executed after modifying the body as changing the innerHTML recreates all the elements and removes the event listeners

    move_to_view("smooth");
}




function unload(){
    let e = document.getElementById('main-mask');
    e.style.pointerEvents = 'all';
    e.style.opacity = '100%';
}



