
var summary_list = {
    init : function(){
        var c = document.getElementsByClassName('summary-list');
        for(var i = 0; i < c.length; ++i){

            let max = 0;
            let li = c[i].getElementsByTagName('LI');
            for(var j = 0; j < li.length; ++j){
                if(!li[j].getElementsByTagName('SPAN').length) console.error("<span> elements not found int summary list <li>");
                max = Math.max(max, li[j].getElementsByTagName('SPAN')[0].clientWidth);
                console.log(li[j].getElementsByTagName('SPAN')[0]);
            }

            for(var j = 0; j < li.length; ++j){
                let span = li[j].getElementsByTagName('SPAN')[0];
                let span2 = li[j].getElementsByTagName('SPAN')[1];
                let maxWidth = `${ max + 20 }px`;

                span.style.maxWidth = maxWidth;
                span.style.minWidth = maxWidth;
                span2.style.maxWidth = maxWidth = `calc(100% - ${maxWidth})`;
                span2.style.minWidth = maxWidth = `calc(100% - ${maxWidth})`;
                li[j].innerHTML =
                    `<div>` +
                        li[j].innerHTML +
                    `</div>`
                ;
            }
        }
    }
};