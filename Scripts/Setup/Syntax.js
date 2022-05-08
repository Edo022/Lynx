

var setup_syntax = {
    // Align the colums to the maximum width of their cells
    even_widths : function(){
        let c = document.getElementsByTagName('SYNTAX2-');
        for(let i = 0; i < c.length; ++i){

            let tables = c[i].getElementsByTagName('TABLE');
            for(let j = 0; j < tables.length; ++j){
                let trs = tables[j].getElementsByTagName('TR');

                // Calculate max width
                let max = Array();
                for(let k = 0; k < trs.length; ++k) {

                    let tds = trs[k].getElementsByTagName('TD');
                    for(let l = 0; l < tds.length; ++l){
                        if(Object.is(max[l], undefined)) {
                            max[l] = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-w').slice(0, -2));
                        }
                        if(max[l] < tds[l].clientWidth) max[l] = tds[l].clientWidth;
                    }
                }

                // Set max width
                for(let k = 0; k < trs.length; ++k) {
                    let tds = trs[k].getElementsByTagName('TD');
                    for(let l = 0; l < tds.length; ++l){
                        tds[l].style.width = `${ max[l] }px`;

                        // Also add a wrapper div, cause why not?
                        // if(tds[l].get)
                        let e = tds[l].childNodes;
                        if(e.length) e[0].innerHTML =
                            '<div>' +
                                e[0].innerHTML +
                            '</div>'
                        ;
                    }
                }
            }
        }
    },




    // Fix syntax codes having different heights because any CSS solution doesn't work and i don't want to add a million wrapper tags
    even_heights : function(){
        let scroll_fix = 16;
        let c = document.getElementsByTagName('SYNTAX2-');
        for(let i = 0; i < c.length; ++i){
            let l = c[i].getElementsByTagName('LEFT2-');
            let r = c[i].getElementsByTagName('RIGHT2-');

            if(l.length) {
                let h = `${Math.max(l[0].clientHeight, r[0].clientHeight) + scroll_fix }px`;
                l[0].style.height = h;
                r[0].style.height = h;
            }
        }
    },




    // Create syntax path arrows with svg elements
    format_arrows : function(){
        // Calculate constants
        let margin = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-margin').slice(0, -2));
        let arrow_d = 5;                                                                                                 //!^ remove 'px' suffix
        let r = 0.05, fix = 0.04;


        // For each tag
        let tags = [
            'TR',  'TB',  'TL',  'RB',  'RL',  'RT',  'BL',  'BT',  'BR',  'LT',  'LR',  'LB',
            'TRC', 'TBC', 'TLC', 'RBC', 'RLC', 'RTC', 'BLC', 'BTC', 'BRC', 'LTC', 'LRC', 'LBC',
            'C'
        ];
        for(let i = 0; i < tags.length; ++i) {
            var children = Array.from(document.getElementsByTagName(`B-${ tags[i] }-`));


            // For each element
            for(let j = 0; j < children.length; ++j) {
                let tagName = children[j].tagName;
                let w = children[j].parentElement.clientWidth - margin * 2;
                let h = children[j].clientHeight;
                let s = '';
                let f = tagName.charCodeAt(2);
                let t = tagName.charCodeAt(3);


                // Connector
                if(tagName[2] == 'C') {
                    s =
                        `M${ w * (1 - fix) },${ 0.5 * h }` +
                        `l${ (margin + w * fix) * 2 },0`
                    ;
                }


                // Arrows
                else {
                    // Calculate coordinates
                    let a = Array();
                        a['T'.charCodeAt(0)] = Math.PI * 1.5; //! y is inverted
                        a['R'.charCodeAt(0)] = Math.PI * 0.0;
                        a['B'.charCodeAt(0)] = Math.PI * 0.5; //! y is inverted
                        a['L'.charCodeAt(0)] = Math.PI * 1.0;
                    let
                        ax0 = Math.cos(a[f]) * (1 + fix), ax = ax0 / 2 + 0.5,
                        ay0 = Math.sin(a[f]) * (1 + fix), ay = ay0 / 2 + 0.5,
                        bx0 = Math.cos(a[t]) * (1 + fix), bx = bx0 / 2 + 0.5,
                        by0 = Math.sin(a[t]) * (1 + fix), by = by0 / 2 + 0.5,
                        h0x = Math.cos(a[t] + Math.PI * 1.25),
                        h0y = Math.sin(a[t] + Math.PI * 1.25),
                        h1x = Math.cos(a[t] - Math.PI * 1.25),
                        h1y = Math.sin(a[t] - Math.PI * 1.25)
                    ;

                    // Draw arrow
                    s =
                        `M${ ax * w },${ ay * h }` +                                                    // Starting position
                        `L${ (ax - ax0 * r) * w },${ (ay - ay0 * r) * h }` +                            // Draw starting segment
                        `Q${ 0.5 * w },${ 0.5 * h } ${ (bx - bx0 * r) * w },${ (by - by0 * r) * h }` +  // Draw curve
                        `L${ bx * w },${ by * h }`                                                      // Draw ending segment
                    ;
                    if(tagName[4] != 'C') s +=
                        `m${ h0x * arrow_d },${ h0y * arrow_d }` +                                      // Starting position
                        `L${ bx * w },${ by * h }` +                                                    // Draw head form 0 to O
                        `l${ h1x * arrow_d },${ h1y * arrow_d }`                                        // Draw head from O to 1
                    ;
                }


                // Add HTML boilerplate
                //FIXME this takes like 99% of the loading time for whatever reason
                children[j].innerHTML =
                    `<svg width="${ w }" height="${ h }">` +
                        `<path d="${ s }"style="stroke: var(--syntax-fg-arrow); stroke-width: 2px; fill: none;"/>` +
                    `</svg>`
                ;
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
            s[i] = (i == 0 || i == s.length - 1) ? '' : '<span class="hidden">' +(('0' + i).slice(-2)) + '&nbsp&nbsp;</span>';
            s[i] += line.substring(min, line.length);
        }

        // Join and return all lines
        return s.join('<br>');
    },



    // Fix code indentation because apparently HTML5+CSS3 can't do that
    format_code : function() {
        let c = document.getElementsByTagName('EXAMPLE2-');
        for(let i = 0; i < c.length; i++){
            let div = c[i].getElementsByTagName('DIV')[0];
            div.innerHTML = setup_syntax.format(div.innerHTML) + '<br>';
            // Add trailing newline to fix CSS's bugged max-height  ^
        }
    },







    init : function() {
        setup_syntax.even_widths();
        setup_syntax.format_arrows();
        setup_syntax.even_heights();
        setup_syntax.format_code();
    }
}