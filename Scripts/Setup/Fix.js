

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}





var setup_fix = {
    // Add 0 width <sup> and <sub> elements to <elm-> tags as they usually have a <sup> tag inside and it screws up the vertical alignment
    fix_elm_height : function(){
        let children = document.getElementsByTagName('ELM-');
        for(let i = 0; i < children.length; i++){
            children[i].innerHTML +=
                '<sup class="nobefore noafter" style="display: inline; margin-left: -100%;"></sup>' +
                '<sub class="nobefore noafter" style="display: inline; margin-left: 100%;"></sub>'
            ;
        }
    },




    init : function() {
        setup_fix.fix_elm_height();
    }
}