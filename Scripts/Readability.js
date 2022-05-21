

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
            if(/^\s*$/.test(n.textContent)) continue; // Skip empty elements
            let w = n.textContent.split(/(?=\b)/);
            let span = document.createElement('span');
            for(let j = 0; j < w.length; ++j){

                let text = w[j];
                // Skip non-alphabetic elements
                if(/[^a-zA-Z]+/.test(text)) {
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
    },




    toggle : function(){
        localStorage.setItem('readability', localStorage.getItem('readability') != 'true');
        location.reload();
    },




    spanw_button : function(){
        document.body.getElementsByTagName('RIGHT-')[0].innerHTML +=
            '<div class="readability-button" onclick="readability.toggle()">' +
            '<span class="a">A</span>' +
            '<span class="b">a</span>' +
            '</div>'
        ;
    },




    load_css : function(s){
        let link = `<link rel="stylesheet" type="text/css" href="Styles/Custom/Readability${ s }.css" media="screen">`;
        document.getElementsByTagName("head")[0].innerHTML += link;
    },




    init : function(){
        r = localStorage.getItem('readability') == 'true';
        readability.load_css(r ? 'On' : 'Off');
        if(r) readability.highlight();

        readability.spanw_button();
    }
}