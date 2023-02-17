index_indent = '3ch';



function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}



var setup_index = {
    // Indent and enumerate index elements
    format_elm : function(elm, depth, last){
        let children = elm.children;
        let separators = 0;
        for(let i = 0; i < children.length; i++){
            let c = children[i];
            if(c.tagName == 'INDEX-SEPARATOR-') {
                separators += 1;
                continue;
            }
            if(c.tagName == 'INDEXD-' || c.tagName == 'INDEXH-') {
                let id = c.innerHTML;
                if(id.length == 0) {
                    c.style.minHeight = '2em';
                    continue;
                }
                let name = capitalize(id.split('.').pop()).replaceAll('-', ' ');
                let num = (c.tagName == 'INDEXH-' ? last : (last + (i - separators) + '.'));

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
            }
            else if(c.tagName == 'INDEX-') {
                setup_index.format_elm(c, depth + 1, last + (i - separators) + '.');
            }
        }
    },


    format : function(){
        setup_index.format_elm(document.querySelector('index-'), 0, '');
    },









    is_id_defined : function(id) {
        return !(Object.is(id, undefined) || id == null || !id.length || document.getElementById(id) == null);
    },




    on_location_changed : function() {
        // Reset the old element and highlight the new one
        let old = window.sessionStorage.getItem('index.selected_id');
        let new_ = `index--${ location.hash.slice(1) }`;
        if(new_ == 'index--' || document.getElementById(new_) == null) new_ = 'index--overview'


        if(setup_index.is_id_defined(old)) {
            document.getElementById(old).parentElement.style.removeProperty("background-color");
        }
        window.sessionStorage.setItem('index.selected_id', new_);

        document.getElementById(new_).parentElement.style.backgroundColor = 'var(--bg-index-active)';
    },


    on_scroll_changed : function(h, i) {
        // Reset the old element and highlight the new one
        let old = window.sessionStorage.getItem('index.active_id');
        let new_ = `index--${ h[Math.max(i - 1, 0)].id }`;
        if(setup_index.is_id_defined(old)) {
            document.getElementById(old).parentElement.style.removeProperty("border-color");
        }
        window.sessionStorage.setItem('index.active_id', new_)
        document.getElementById(new_).parentElement.style.borderColor = 'var(--bg-index-active)';
    },




    check_scroll : function(right){
        let h = document.querySelectorAll('h1');
        for(var i = 0; i < h.length; i++) {

            // Check if element is in view
            let view = right.getBoundingClientRect();
            if(h[i].getBoundingClientRect().top >= view.top + parseInt(getComputedStyle(document.body).getPropertyValue('--sep-4'))) {
                setup_index.on_scroll_changed(h, i);
                return;
            }
        }
    },




    init : function(){
        setup_index.format();
        setup_index.on_location_changed();


        // Setup scroll listener
        right.addEventListener('scroll', function(){ setup_index.check_scroll(right); });


        // Setup location change listener
        window.addEventListener('hashchange', function(e){ setup_index.on_location_changed(); });
    }
}