

function decodeHTML(html) {
    let txt = document.createElement("textarea");
    txt.innerHTML = html;
    return txt.value;
}

function copy_code(lable) {
    let children = lable.parentNode.childNodes;
    for(let i = 0; i < children.length; i++){
        if(children[i].tagName == "CODE-") {
            let output_text = decodeHTML(children[i].innerHTML.replace(/<\/?(!--|([a-zA-Z0-9_\-]+))[^>]+>/g, ""));
            output_text = output_text.substring(1, output_text.length - 2).split('\n');
            //           Remove leading newline ^                       ^
            //             Remove trailing newline and CSS fix newline -'
            for(let i = 0; i < output_text.length; i++) {
                output_text[i] = output_text[i].slice(4);
            }
            navigator.clipboard.writeText(output_text.join("\n"));
            return 0;
        }
    }
}


function initCopyCode() {
    let children = document.getElementsByTagName("LABEL-");
    for(let i = 0; i < children.length; i++){
        children[i].addEventListener("click", function(){ copy_code(this); })
    }
};