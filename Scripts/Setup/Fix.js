

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}





var setup_fix = {
    // Add 0 width <sup> and <sub> elements to <elm-> tags as they usually have a <sup> tag inside and it screws up the vertical alignment
    fix_elm_height : function(){
        let c = document.querySelectorAll('elm-');
        for(let i = 0; i < c.length; i++){
            c[i].innerHTML +=
                '<sup class="nobefore noafter" style="display: inline; margin-left: -100%;"></sup>' +
                '<sub class="nobefore noafter" style="display: inline; margin-left: 100%;"></sub>'
            ;
        }
    },




    fix_split_td_height : function(){
        let c = document.querySelectorAll('.split-td-outer');
        for(let i = 0; i < c.length; ++i) {
            let e = document.createElement('div');
            e.classList = 'split-td-outer2';

            let outerHTML = c[i].outerHTML;
            c[i].replaceWith(e);
            e.innerHTML = outerHTML;
        }
        c = document.querySelectorAll('.split-td');
        for(let i = 0; i < c.length; ++i) {
            c[i].innerHTML = `<div><div>${ c[i].innerHTML }</div></div>`;
        }
    },




    init : function() {
        setup_fix.fix_elm_height();
        setup_fix.fix_split_td_height();
    }
}