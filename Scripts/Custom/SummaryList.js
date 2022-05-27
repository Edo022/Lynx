
var summary_list = {
    init : function(){
        var c = document.querySelectorAll('.summary-list');
        for(var i = 0; i < c.length; ++i){

            let max = 0;
            let li = c[i].querySelectorAll('li');
            for(var j = 0; j < li.length; ++j){
                if(!li[j].querySelectorAll('span').length) console.error("<span> elements not found in summary list <li>");
                max = Math.max(max, li[j].querySelector('span').clientWidth);
            }

            for(var j = 0; j < li.length; ++j){
                let spans = li[j].querySelectorAll('span');
                let maxWidth = `${ max + 20 }px`;

                spans[0].style.maxWidth = maxWidth;
                spans[0].style.minWidth = maxWidth;
                spans[1].style.maxWidth = maxWidth = `calc(100% - ${maxWidth})`;
                spans[1].style.minWidth = maxWidth = `calc(100% - ${maxWidth})`;
                li[j].innerHTML =
                    `<div>` +
                        li[j].innerHTML +
                    `</div>`
                ;
            }
        }
    }
};