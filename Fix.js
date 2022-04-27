function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}






// Remove HTML indentation from pre blocks and add the line number
function format(s){
    // Find shortest indentation
    let min;
    s = s.split('\n');
    for(let i = 0; i < s.length; i++){
        let line = s[i];
        let spaces = line.search(/\S/);
        if((min > spaces || min == undefined) && spaces >= 0){
            min = spaces;
        }
    }
    // Remove indentation
    for(let i = 0; i < s.length; i++){
        let line = s[i];
        s[i] = (i == 0 || i == s.length - 1) ? "" : "<span class=\"hidden\">" +(("0" + i).slice(-2)) + '&nbsp&nbsp;</span>';
        s[i] += line.substring(min, line.length);
    }

    // Join and return all lines
    return s.join("\n");
}



// Fix code indentation because apparently HTML5+CSS3 can't do that
function format_code() {
    let children = document.getElementsByTagName("EXAMPLE-");
    for(let i = 0; i < children.length; i++){
        let children2 = children[i].getElementsByTagName("SCROLL-");
        for(let j = 0; j < children2.length; ++j){
            children2[j].innerHTML = format(children2[j].innerHTML) + "\n";
            // Add trailing newline to fix CSS's bugged max-height  ^
        }
    }
}



// Fix syntax codes having different heights because any CSS solution doesn't work and i don't want to add a million wrapper tags
function fix_syntax_heights(){
    let children = document.getElementsByTagName("LEFT-");
    for(let i = 0; i < children.length; i++){
        let left = children[i];
        if(left.parentNode.tagName == "SYNTAX-" || left.parentNode.tagName == "EXAMPLE-") {
            let right;
            let children2 = left.parentNode.children;
            for(let j = 0; j < children2.length; j++){
                if(children2[j].tagName == 'RIGHT-') {
                    right = children2[j];
                    break;
                }
            }
            let max = Math.max(left.clientHeight, right.clientHeight);
            left.style.height = "" + max + "px";
            right.style.height = "" + max + "px";
        }
    }
}


// Add 0 width <sup> and <sub> elements to <elm-> tags as they usually have a <sup> tag inside and it screws up the vertical alignment
function fix_elm_height(){
    let children = document.getElementsByTagName("ELM-");
    for(let i = 0; i < children.length; i++){
        children[i].innerHTML +=
            "<sup class=\"nobefore noafter\" style=\"display: inline; margin-left: -100%;\"></sup>" +
            "<sub class=\"nobefore noafter\" style=\"display: inline; margin-left: 100%;\"></sub>"
        ;
    }
}



// Indent and enumerate index elements
let index_indent = "3ch";
function format_index_elm(elm, depth, last){
    let children = elm.children;
    for(let i = 0; i < children.length; i++){
        let c = children[i];
        if(c.tagName == "INDEXD-" || c.tagName == "INDEXH-") {
            let id = c.innerHTML;
            if(id.length == 0) {
                c.style.minHeight = "2em";
                continue;
            }
            let name = capitalize(id).replaceAll('-', ' ');
            let num = (c.tagName == "INDEXH-" ? last : last + i + ".");

            // Fix index
            c.innerHTML =
                "<a style=\"display: inline-block; padding: 0 1ch 0 1ch; color: var(--fg-link);\" " +
                "href=\"#" + id + "\">" + num + " " + name + "</a>"
            ;
            c.style.paddingLeft = "calc(" + index_indent + " * " + (depth - (c.tagName == "INDEXH-")) + ")";
            c.style.maxWidth = "calc(100% - " + index_indent + " * " + (depth - (c.tagName == "INDEXH-")) + ")";

            //Fix header
            console.info("loaded " + id);
            let depth2 = (c.tagName == "INDEXH-" ? depth : depth + 1);
            let header = document.getElementById(id);

            header.insertAdjacentHTML("beforebegin", "<sep-" + depth2 + "-></sep-" + depth2 + "->");
            header.innerHTML = "" + num + " " + name;
            if(depth == 0) header.insertAdjacentHTML("afterend", "<sep-3-></sep-3->");
            header.classList.add("h" + depth2);
        }
        else if(c.tagName == "INDEX-") {
            format_index_elm(c, depth + 1, last + i + ".");
        }
    }
}
function format_index(){
    let children = document.getElementsByTagName("INDEX-");
    format_index_elm(children[0], 0, "");
}




// Create syntax path arrows with svg elements
function format_arrows(){
    var arrow_d = 3;


    // Format elements
    var children =
        Array.from(document.getElementsByTagName('MERGE-UP-'))
        .concat(Array.from(document.getElementsByTagName('MERGE-MID-')))
        .concat(Array.from(document.getElementsByTagName('MERGE-DOWN-')))
    ;
    for(var i = 0; i < children.length; ++i) {
        var w = children[i].clientWidth;
        var h = children[i].clientHeight * 2;
        var t = (children[i].tagName == 'MERGE-MID-') ? 0 : ((children[i].tagName == 'MERGE-UP-') ? 1 : -1);
        children[i].innerHTML =
            '<svg width="' + w + '" height="' + h + '"' + ((t == 1) ? (', style="margin-top: ' + (-h / 2) + 'px;"') : '') + '>' +
                '<path d="M0,' + (h * (0.25 + 0.5 * (t == 1))) +
                ' l' + (w * 0.10) + ',0 c' + (w * 0.40) + ',0 0,' + (h * 0.5 * -t) + ' ' + (w * 0.40) + ',' + (h * 0.5 * -t) + ' l' + (w * 0.10) + ',0' +
                ' l' + (arrow_d * -1) + ',' + (arrow_d * -1) + ' m' + (arrow_d * 1) + ',' + (arrow_d * 1) + ' l' + (arrow_d * -1) + ',' + (arrow_d * 1) + '' +
                '"style="stroke: var(--fg-arrow); stroke-width: 1px; fill: none;"/>' +
            '</svg>'
        ;
    }


    // Wrap lines in divs to control the line height
    // The div is styled from CSS
    children = document.getElementsByTagName('SYNTAX-');
    for(var i = 0; i < children.length; ++i) {
        var children2 = children[i].getElementsByTagName('TD');
        for(var j = 0; j < children2.length; ++j){
            children2[j].innerHTML = '<div>' + children2[j].innerHTML + '</div>';
        }
    }
}




function initFix() {
    // Front end moment
    format_index();
    format_code();
    format_arrows();
    fix_elm_height();
    fix_syntax_heights();


    // This scrolls to the correct header, which is the same the browser already scrolled to.
    // Apparently, JS is loaded after scrolling to it and adding new HTML elements messes everything up,
    // so JS has to scroll again after the new elements are loaded.
    // Anything else refuses to work
    let header = window.location.href;
    window.location.replace(header);


    // The id="main-mask" div is used to hide the page before js is done moving stuff around as anything else just doesn't work
    // This line removes it from the body so that the user can see the page and think it loaded flawlessly
    document.body.removeChild(document.getElementById('main-mask'));
};