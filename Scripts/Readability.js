
let readability_highlighted = false;

var readability = {

    // Returns all the text nodes in the document
    textNodesUnder : function(el){
        var n, a=[], walk=document.createTreeWalker(el, NodeFilter.SHOW_TEXT, null, false);
        while(n=walk.nextNode()) a.push(n);
        return a;
    },


    // Highlights half of each word to make them more readable
    highlight : function(){
        let walk = document.createTreeWalker(document.documentElement, NodeFilter.SHOW_TEXT, null, false);
        let n, lastOld = null, lastNew = null;
        while(n = walk.nextNode()) {

            // Load new elements
            if(/^\s*$/.test(n.textContent) | n.parentElement.tagName == 'TITLE' | n.parentElement.tagName == 'STYLE') continue; // Skip empty elements, tab title and styles
            let w = n.textContent.split(/((?<=[a-z]+)(?=[^a-z]+)|(?<=[^a-z]+)(?=[a-z]+))/i);
            //!                           ^ word-to-symbol       ^ symbol-to-word
            let span = document.createElement('span');
            for(let j = 0; j < w.length; ++j){

                let text = w[j];
                // Skip symbolic elements
                if(/[^a-z]/i.test(text[0])) {
                    let a = document.createElement('span');
                    a.innerHTML = text;
                    span.appendChild(a);
                }
                else {
                    let mid = Math.ceil(text.length / 2);

                    // Save the 2 parts as separate elements
                    let a = document.createElement('b-');
                    let b = document.createElement('span');
                    a.innerHTML = text.slice(0, mid);
                    b.innerHTML = text.slice(mid, text.length);

                    // Append them if they are not empty
                    if(a.innerHTML.length) span.appendChild(a);
                    if(b.innerHTML.length) span.appendChild(b);
                }
            }

            // Replace old elements
            //! Save the current node and replacement and *use them in the next iteration*
            //! Replacing the current node breaks TreeWalker
            if(lastOld != null) lastOld.replaceWith(lastNew);
            lastOld = n;
            lastNew = span;
        }
        // Replace last element
        if(lastOld != null) lastOld.replaceWith(lastNew);
    },




    toggle : function(){
        localStorage.setItem('readability', localStorage.getItem('readability') != 'true');
        readability.load_css();
        // move_to_view();
    },




    spanw_button : function(){
        document.body.querySelector('right-').innerHTML +=
            '<div class="readability-button" onclick="readability.toggle()">' +
                '<span class="a">A</span>' +
                '<span class="b">a</span>' +
            '</div>'
        ;
    },




    load_css : function(){
        let r = localStorage.getItem('readability') == 'true';
        let linkStr = `Styles/Custom/Readability${ r ? 'On' : 'Off' }.css`;

        let e = document.getElementById('readability-css');
        if(e == null) {
            // Load style
            var link = document.createElement('link');
            link.setAttribute("rel", "stylesheet");
            link.setAttribute("type", "text/css");
            link.onload = view;
            link.setAttribute("href", linkStr);
            link.id = 'readability-css';
            document.querySelector("head").appendChild(link);
        }
        else {
            e.setAttribute('href', linkStr)
            e.onload = null;
        }

        if(!readability_highlighted) {
            readability.highlight();
            readability_highlighted = true;
        }
    },




    init : function(){
        readability.load_css();
        readability.spanw_button();
    }
}