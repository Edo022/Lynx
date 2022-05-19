

function init() {
    ui_slider.init();
        setup_index.init();
        setup_fix.init();
        setup_syntax.init();
    ui_copy_code.init();

    summary_list.init();


    // This scrolls to the correct header, which is the same the browser already scrolled to.
    // Apparently, JS is loaded after scrolling to it and adding new HTML elements messes everything up,
    // so JS has to scroll again after the new elements are loaded.
    // Anything else refuses to work
    let header = window.location.href;
    window.location.replace(header);


    // The id="main-mask" div is used to hide the page before js is done moving stuff around as anything else just doesn't work
    // This line removes it from the body so that the user can see the page and think it loaded flawlessly
    document.body.removeChild(document.getElementById('main-mask'));
}
