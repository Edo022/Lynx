

var setup_syntax = {
    min_w : parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-w')),



    // Align the colums to the maximum width of their cells
    even_widths : function(){
        let min_w = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-w'));
        let tables = document.querySelectorAll('syntax- > table');
        for(let j = 0; j < tables.length; ++j){
            let trs = tables[j].querySelectorAll('tr');


            // Calculate max width
            let max = Array();
            for(let k = 0; k < trs.length; ++k) {

                let tds = trs[k].querySelectorAll('td');
                for(let l = 0; l < tds.length; ++l){
                    if(Object.is(max[l], undefined)) {
                        max[l] = setup_syntax.min_w;
                    }
                    if(max[l] < tds[l].offsetWidth) max[l] = tds[l].offsetWidth;
                }
            }


            // Set width
            for(let k = 0; k < trs.length; ++k) {
                let tds = trs[k].querySelectorAll('td');
                for(let l = 0; l < tds.length; ++l){

                    // Resize elements and wrap child elements into a div
                    let e = tds[l].childNodes;
                    if(e.length) {
                        tds[l].style.width = `${ max[l] }px`;
                        e[0].innerHTML = `<div>${ e[0].innerHTML }</div>`;
                    }

                    // Stretch arrows to fit long td bois
                    else {
                        tds[l].style.width = `${ min_w }px`;
                        let ratio = max[l] / setup_syntax.min_w;
                        tds[l].style.transform   = `scaleX(${ ratio + 2 * (1 / setup_syntax.min_w) })`; // Add an extra pixel on each side to connect the arrows seamlessly
                        tds[l].style.marginLeft  = `${ (ratio - 1) / 2 * min_w }px`;
                        tds[l].style.marginRight = `${ (ratio - 1) / 2 * min_w }px`;
                    }
                }
            }
        }
    },











    format_arrows : function(){
        c = right.querySelectorAll("syntax- > table td");


        // Fix inverted arrows
        for(let i = 0; i < c.length; ++i){
            if(c[i].dataset.arrows && c[i].dataset.arrows.length) c[i].dataset.arrows = c[i].dataset.arrows
                .replaceAll(/\bbt\b/g, "tb")
                .replaceAll(/\brl\b/g, "lr")
                .replaceAll(/\btl\b/g, "lt")
                .replaceAll(/\brt\b/g, "tr")
                .replaceAll(/\bbr\b/g, "rb")
                .replaceAll(/\blb\b/g, "bl")
            ;
        }


        // Add automatic connectors
        for(let i = 0; i < c.length; ++i){
            if(c[i].dataset.arrows && c[i].dataset.arrows.length){

                // Vertical connectors
                if(/\b(tb|lt|tr)\b/.test(c[i].dataset.arrows)) {
                    let child_index = function(obj){
                        let p = obj.previousElementSibling;
                        return p ? child_index(p) + 1 : 0;
                    };
                    let prev_tr = c[i].parentElement.previousElementSibling;
                    if(prev_tr) {
                        let prev = prev_tr.children[child_index(c[i])];
                        if(prev) {
                            if(prev.dataset.arrows && /\b(tb|rb|bl)\b/.test(prev.dataset.arrows)) {
                                prev.dataset.arrows += " connect-b";
                                c[i].dataset.arrows += " connect-t";
                            }
                        }
                    }
                }

                // Horizontal connectors
                let cur_low  = /\b(lr|lt|bl)-low\b/     .test(c[i].dataset.arrows);
                let cur_std  = /\b(lr|lt|bl)\b($|[^\-])/.test(c[i].dataset.arrows);
                let cur_high = /\b(lr|lt|bl)-high\b/    .test(c[i].dataset.arrows);
                if(cur_low | cur_std | cur_high) {
                    let prev = c[i].previousElementSibling;
                    if(prev) {
                        if(prev.dataset.arrows && prev.dataset.arrows.length) {
                            let prev_low  = /\b(lr|tr|rb)-low\b/     .test(prev.dataset.arrows);
                            let prev_std  = /\b(lr|tr|rb)\b($|[^\-])/.test(prev.dataset.arrows);
                            let prev_high = /\b(lr|tr|rb)-high\b/    .test(prev.dataset.arrows);
                            if(cur_low && prev_low) {
                                prev.dataset.arrows += " connect-r-low";
                                c[i].dataset.arrows += " connect-l-low";
                            }
                            if(cur_std && prev_std) {
                                prev.dataset.arrows += " connect-r";
                                c[i].dataset.arrows += " connect-l";
                            }
                            if(cur_high && prev_high) {
                                prev.dataset.arrows += " connect-r-high";
                                c[i].dataset.arrows += " connect-l-high";
                            }
                        }
                    }
                }
            }
        }


        // Set backgrounds
        for(let i = 0; i < c.length; ++i){
            if(c[i].dataset.arrows && c[i].dataset.arrows.length){
                let bg = "";
                let arrows = c[i].dataset.arrows.split(/[\s]+/);
                for(let j = 0; j < arrows.length; ++j) bg += `url("./Styles/Blocks/Syntax/Arrows/${ arrows[j] }.svg") center/contain no-repeat content-box, `;
                c[i].style.background = bg.slice(0, -2);
            }
        }
    },









    // Remove HTML indentation from pre blocks and add the line number
    format : function(s){
        // Find shortest indentation
        let min;
        s = s.split('\n');
        for(let i = 0; i < s.length; i++){
            let line = s[i];
            let spaces = line.search(/\S/);
            if((min > spaces || min == undefined) && spaces >= 0){
                min = spaces;
            }
        }
        // Remove indentation
        for(let i = 0; i < s.length; i++){
            let line = s[i];
            s[i] = (i == 0 || i == s.length - 1) ? '' : '<span class="hidden">' +(('0' + i).slice(-2)) + '&nbsp;&nbsp;</span>';
            s[i] += line.substring(min, line.length);
        }

        // Join and return all lines
        return s.join('<br>');
    },



    // Fix code indentation because apparently HTML5+CSS3 can't do that
    indent_code : function() {
        let c = document.querySelectorAll('example-');
        for(let i = 0; i < c.length; i++){
            let divs = c[i].querySelectorAll('div');
            for(let j = 0; j < divs.length; ++j){
                divs[j].innerHTML = setup_syntax.format(divs[j].innerHTML) + '<br>';
                // Add trailing newline to fix CSS's bugged max-height  ^
            }
        }
    },





    // Even out the height of right and left example tags
    even_heights : function() {
        let c = document.querySelectorAll('split-example-container-');

        for(let i = 0; i < c.length; i++){
            // Find left and right containers
            let lc = c[i].querySelector('split-example-container-left-');
            let rc = c[i].querySelector('split-example-container-right-');

            // Get contained divs
            let l = lc.querySelector('DIV');
            let r = rc.querySelector('DIV');

            // Fix heights
            if(r.offsetHeight < l.offsetHeight) r.style.minHeight = r.style.maxHeight = `${ l.offsetHeight }px`
            if(l.offsetHeight < r.offsetHeight) l.style.minHeight = l.style.maxHeight = `${ r.offsetHeight }px`
        }
    },







    init : function() {
        log_function(setup_syntax.even_widths,   "    even_widths"  );
        log_function(setup_syntax.format_arrows, "    format_arrows");
        log_function(setup_syntax.indent_code,   "    indent_code"  );
        log_function(setup_syntax.even_heights,  "    even_heights" );
    }
}
