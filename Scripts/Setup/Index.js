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
                let name = capitalize(id.split('.').pop()).replaceAll('-', ' ');
                let num = (c.tagName == 'INDEXH-' ? last : (last + i + '.'));

                // Fix index
                c.innerHTML =
                    `<a ` +
                        `style="display: inline-block; padding: 0 1ch 0 1ch; color: var(--fg-link);" ` +
                        `href="#${ id }"` +
                        `id="index--${ id }"` +
                    `>${ num } ${ name }</a>`
                ;
                c.style.paddingLeft = `calc(` +   `${ index_indent } * ${ (depth - (c.tagName == 'INDEXH-')) })`;
                c.style.maxWidth    = `calc(100% - ${ index_indent } * ${ (depth - (c.tagName == 'INDEXH-')) })`;

                //Fix header
                let depth2 = (c.tagName == 'INDEXH-' ? depth : depth + 1);
                let header = document.getElementById(id);
                if(header == null) console.error(`header for ID ${ id } not found`);

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
        setup_index.format_elm(document.querySelector('index-'), 0, '');
    },




    on_location_changed : function(e) {
        console.log(location.hash);
    },







    check_active : function(active_id) {
        return !(Object.is(active_id, undefined) || active_id == null || !active_id.length || document.getElementById(active_id) == null);
    },


    check_scroll : function(right){
        let h = document.querySelectorAll('h1');
        for(var i = 0; i < h.length; i++) {

            // Check if element is in view
            let view = right.getBoundingClientRect();
            if(h[i].getBoundingClientRect().top >= view.top + parseInt(getComputedStyle(document.body).getPropertyValue('--sep-4'))) {

                // Reset the old element and highlight the new one
                let old = window.sessionStorage.getItem('active_index');
                let new_ = `index--${ h[Math.max(i - 1, 0)].id }`;
                if(setup_index.check_active(old)) {
                    document.getElementById(old).parentElement.style.removeProperty('background-color');
                }
                window.sessionStorage.setItem('active_index', new_)
                document.getElementById(new_).parentElement.style.backgroundColor = 'var(--bg-index-active)';

                return;
            }
        }
    },




    init : function(){
        setup_index.format();

        // Setup scroll listener
        let right = document.querySelector('right-');
        right.addEventListener('scroll', function(){ setup_index.check_scroll(right); });

        // Setup location change listener
        window.addEventListener("beforeunload", setup_index.on_location_changed);
    }
}