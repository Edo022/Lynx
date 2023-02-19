
let readability_highlighted = false;
let rec_i = 0;

var readability = {
    // Highlights half of each word to make them more readable
    highlight : function(){
        let walk = document.createTreeWalker(
            document.documentElement,
            NodeFilter.SHOW_TEXT,
            { acceptNode(node) {
                checkParent = function(node){
                    if(node.parentElement == null) return NodeFilter.FILTER_ACCEPT;
                    switch(node.parentElement.tagName) {
                        case 'EXAMPLE-': return NodeFilter.FILTER_REJECT;
                        case 'SYNTAX-':  return NodeFilter.FILTER_REJECT;
                        case 'TITLE':    return NodeFilter.FILTER_REJECT;
                        case 'STYLE':    return NodeFilter.FILTER_REJECT;
                        case 'CODE':     return NodeFilter.FILTER_REJECT;
                        default:         return checkParent(node.parentElement);
                    }
                }

                // Skip empty elements
                if(/^\s*$/.test(node.textContent)) return NodeFilter.FILTER_REJECT;
                return checkParent(node);
            }}
        );



        let n = walk.nextNode();
        while(n) {
            // Load new elements
            let w = n.textContent.split(/((?<=[a-z]+)(?=[^a-z]+)|(?<=[^a-z]+)(?=[a-z]+))/i);
            //!                           ^ word-to-symbol       ^ symbol-to-word
            let c = document.createElement('txt-');
            let has_text = false;
            for(let j = 0; j < w.length; ++j){

                let text = w[j];
                // Skip symbolic elements
                if(/[^a-z]/i.test(text[0])) {
                    let a = document.createElement('a-');
                    a.innerHTML = text;
                    c.appendChild(a);
                }
                else {
                    let mid = Math.ceil(text.length / 2);

                    // Save the 2 parts as separate elements
                    let a = document.createElement('a-');
                    let b = document.createElement('b-');
                    a.innerHTML = text.slice(0, mid);
                    b.innerHTML = text.slice(mid, text.length);

                    // Append them if they are not empty
                    if(a.innerHTML.length && b.innerHTML.length) has_text = true;
                    if(a.innerHTML.length) c.appendChild(a);
                    if(b.innerHTML.length) c.appendChild(b);
                }
            };

            // Save current element, walk over it and replace it with the new node
            if(has_text){
                let cur = n;
                n = walk.nextNode()
                cur.replaceWith(c);
            }
            else {
                n = walk.nextNode()
            }
        }
    },




    toggle : function(){
        localStorage.setItem('readability', localStorage.getItem('readability') != 'true');
        readability.load_css();
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
            // link.onload = view;
            link.setAttribute("href", linkStr);
            link.id = 'readability-css';
            let h = document.getElementsByTagName("head")[0];
            h.appendChild(link, h);
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