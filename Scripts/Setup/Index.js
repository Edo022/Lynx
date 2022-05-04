index_indent = '3ch';

var setup_index = {
    // Indent and enumerate index elements
    format_elm : function(elm, depth, last){
        let children = elm.children;
        for(let i = 0; i < children.length; i++){
            let c = children[i];
            if(c.tagName == 'INDEXD-' || c.tagName == 'INDEXH-') {
                let id = c.innerHTML;
                if(id.length == 0) {
                    c.style.minHeight = '2em';
                    continue;
                }
                let name = capitalize(id).replaceAll('-', ' ');
                let num = (c.tagName == 'INDEXH-' ? last : last + i + '.');

                // Fix index
                c.innerHTML =
                    `<a style="display: inline-block; padding: 0 1ch 0 1ch; color: var(--fg-link);" ` +
                    `href="#${ id }">${ num } ${ name }</a>`
                ;
                c.style.paddingLeft = `calc(` +   `${ index_indent } * ${ (depth - (c.tagName == 'INDEXH-')) })`;
                c.style.maxWidth    = `calc(100% - ${ index_indent } * ${ (depth - (c.tagName == 'INDEXH-')) })`;

                //Fix header
                let depth2 = (c.tagName == 'INDEXH-' ? depth : depth + 1);
                let header = document.getElementById(id);

                header.insertAdjacentHTML('beforebegin', `<sep-${ depth2 }-></sep-${ depth2 }->`);
                header.innerHTML = `${ num } ${ name }`;
                if(depth == 0) header.insertAdjacentHTML('afterend', '<sep-3-></sep-3->');
                header.classList.add('h' + depth2);
                console.debug(`[${ id }] - loaded to ${ num }`);
            }
            else if(c.tagName == 'INDEX-') {
                setup_index.format_elm(c, depth + 1, last + i + '.');
            }
        }
    },




    format : function(){
        let children = document.getElementsByTagName('INDEX-');
        setup_index.format_elm(children[0], 0, '');
    },




    init : function(){
        setup_index.format();
    }
}