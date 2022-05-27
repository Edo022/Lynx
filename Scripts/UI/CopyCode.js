

var ui_copy_code = {
    decodeHTML : function(html) {
        let txt = document.createElement("textarea");
        txt.innerHTML = html;
        return txt.value;
    },

    copy_code : function(lable) {
        let children = lable.parentNode.childNodes;
        for(let i = 0; i < children.length; i++){
            if(children[i].tagName == "DIV") {

                // Convert <br> to newlines and decode the lines one by one
                let output_text = children[i].innerHTML.replaceAll(/<br>/g, '\n').split('\n');
                for(let i = 0; i < output_text.length; i++) {
                    output_text[i] = ui_copy_code.decodeHTML(output_text[i].replaceAll(/<\/?(!--|([a-zA-Z0-9_\-]+))[^>]+>/g, '')).slice(4);
                }

                // Join the lines and remove all leading and trailing newlines
                navigator.clipboard.writeText(output_text.join('\n').trim());
                return 0;
            }
        }
    },


    init : function() {
        let children = document.querySelectorAll("label2-");
        for(let i = 0; i < children.length; i++){
            children[i].addEventListener('click', function(){ ui_copy_code.copy_code(this); });
        }
    }
}
