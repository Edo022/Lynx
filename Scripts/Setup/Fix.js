
var setup_fix = {
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
        setup_fix.fix_split_td_height();
    }
}