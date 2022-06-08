
var setup_copy_syntax = {
    copy : function() {

        // For each label
        let l0 = document.querySelectorAll('label2-');
        for(let i = 0; i < l0.length; ++i) {
            let id = l0[i].id.split('.', 2)[1];

            // If it requires a copy
            if(/^copy\..*/.test(l0[i].id)) {

                // For each syntax
                let found = false;
                let s = document.querySelectorAll('syntax2-');
                for(let k = 0; k < s.length && !found; ++k) {

                    // For each child label
                    let l1 = s[k].querySelectorAll('label2-');
                    for(let l = 0; l < l1.length; ++l) {

                        // If id matches
                        if(l1[l].id == id){
                            l0[i].insertAdjacentElement('afterend', l1[l].parentElement.querySelectorAll('table')[l].cloneNode(true));
                            l0[i].classList = l1[l].classList;
                            found = true;
                            break;
                        }
                    }

                    // Append note if present
                    let note = s[k].querySelector('syntax-note-');
                    if(found && note != null && l0[i].parentElement.querySelector('syntax-note-') == null) l0[i].parentElement.appendChild(note.cloneNode(true));
                }

                // Print error if the ID cannot be found
                if(!found) console.error(`syntax not found for id ${ id }`)
            }
        }
    },




    init : function() {
        setup_copy_syntax.copy();
    }
}