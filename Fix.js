function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}






// Remove HTML indentation from pre blocks and add the line number
function format(s){
    // find shortest indentation
    var min;
    s = s.split('\n');
    for(var i = 0; i < s.length; i++){
        var line = s[i];
        var spaces = line.search(/\S/);
        if((min > spaces || min == undefined) && spaces >= 0){
            min = spaces;
        }
    }
    // remove indentation
    for(var i = 0; i < s.length; i++){
        var line = s[i];
        s[i] = (i == 0 || i == s.length - 1) ? "" : "<span class=\"hidden\">" + (("0" + i).slice(-2)) + "&nbsp&nbsp;</span>";
        s[i] += line.substring(min, line.length);
    }

    // join and return all lines
    return s.join("\n");
}



// Fix code indentation because apparently HTML5+CSS3 can't do that
function format_code() {
    var children = document.getElementsByTagName("SCROLL-");
    for(var i = 0; i < children.length; i++){
        children[i].innerHTML = format(children[i].innerHTML) + "\n";
        // Add trailing newline to fix CSS's bugged max-height  ^
    }
}



// Fix syntax codes having different heights because any CSS solution doesn't work and i don't want to add a million wrapper tags
function fix_syntax_heights(){
    var children = document.getElementsByTagName("LEFT-");
    for(var i = 0; i < children.length; i++){
        var left = children[i];
        if(left.parentNode.tagName == "SYNTAX-" || left.parentNode.tagName == "EXAMPLE-") {
            var right;
            var children2 = left.parentNode.children;
            for(var j = 0; j < children2.length; j++){
                if(children2[j].tagName == 'RIGHT-') {
                    right = children2[j];
                    break;
                }
            }
            var max = Math.max(left.clientHeight, right.clientHeight);
            left.style.height = "" + max + "px";
            right.style.height = "" + max + "px";
        }
    }
}


// Add 0 width <sup> and <sub> elements to <elm-> tags as they usually have a <sup> tag inside and it screws up the vertical alignment
function fix_elm_height(){
    var children = document.getElementsByTagName("ELM-");
    for(var i = 0; i < children.length; i++){
        children[i].innerHTML +=
            "<sup class=\"nobefore noafter\" style=\"display: inline; margin-left: -100%;\"></sup>" +
            "<sub class=\"nobefore noafter\" style=\"display: inline; margin-left: 100%;\"></sub>"
        ;
    }
}



// Indent and enumerate index elements
var index_indent = "3ch";
function format_index_elm(elm, depth, last){
    var children = elm.children;
    for(var i = 0; i < children.length; i++){
        var c = children[i];
        if(c.tagName == "INDEXD-" || c.tagName == "INDEXH-") {
            var id = c.innerHTML;
            if(id.length == 0) {
                c.style.minHeight = "2em";
                continue;
            }
            var name = capitalize(id).replaceAll('-', ' ');
            var num = (c.tagName == "INDEXH-" ? last : last + i + ".");

            // Fix index
            c.innerHTML =
                "<a style=\"display: inline-block; padding: 0 1ch 0 1ch;\" " +
                "href=\"#" + id + "\">" + num + " " + name + "</a>"
            ;
            c.style.paddingLeft = "calc(" + index_indent + " * " + (depth - (c.tagName == "INDEXH-")) + ")";
            c.style.maxWidth = "calc(100% - " + index_indent + " * " + (depth - (c.tagName == "INDEXH-")) + ")";

            //Fix header
            console.info("loaded " + id);
            var depth2 = (c.tagName == "INDEXH-" ? depth : depth + 1);
            var header = document.getElementById(id);

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
    var children = document.getElementsByTagName("INDEX-");
    format_index_elm(children[0], 0, "");
}






document.addEventListener("DOMContentLoaded", function() {
    format_index();
    format_code();
    fix_elm_height();
    fix_syntax_heights();

    // This scrolls to the correct header, which is the same the browser already scrolled to.
    // Apparently, JS is loaded after scrolling to it and adding new HTML elements messes everything up,
    // so JS has to scroll again after the new elements are loaded.
    // Anything else refuses to work
    var header = window.location.href;
    window.location.replace(header);
});