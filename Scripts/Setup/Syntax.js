

var setup_syntax = {
    // Align the colums to the maximum width of their cells
    even_widths : function(){
        let tables = document.querySelectorAll('syntax2- > table');
        for(let j = 0; j < tables.length; ++j){
            let trs = tables[j].querySelectorAll('tr');

            // Calculate max width
            let max = Array();
            for(let k = 0; k < trs.length; ++k) {

                let tds = trs[k].querySelectorAll('td');
                for(let l = 0; l < tds.length; ++l){
                    if(Object.is(max[l], undefined)) {
                        max[l] = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-w').slice(0, -2));
                    }
                    if(max[l] < tds[l].clientWidth) max[l] = tds[l].clientWidth;
                }
            }

            // Set max width
            for(let k = 0; k < trs.length; ++k) {
                let tds = trs[k].querySelectorAll('td');
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
    },




    toDecimalPrecision : function(n, digits){
        let int_digits = n.toFixed().length;
        return n.toPrecision(int_digits + digits);
    },



    // Create syntax path arrows with svg elements
    format_arrows : function(){
        // Calculate constants
        let margin = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-margin').slice(0, -2));
        let arrow_d = 5;    // Arrow head size                                                                           //!^ Remove 'px' suffix
        let mfix = 0.3;     // Height difference of height locations
        let r = 0.2;        // Arrow curve radius

        let a = Array();
            a['T'.charCodeAt(0)] = Math.PI * 1.5; //! T is inverted
            a['R'.charCodeAt(0)] = Math.PI * 0.0;
            a['B'.charCodeAt(0)] = Math.PI * 0.5; //! T is inverted
            a['L'.charCodeAt(0)] = Math.PI * 1.0;
        ;


        // For each tag
        let children = document.querySelectorAll(
            'B-TR-,   B-TB-,   B-TL-,   B-RB-,   B-RL-,   B-RT-,   B-BL-,   B-BT-,   B-BR-,   B-LT-,   B-LR-,   B-LB-, '   +
            'B-TRC-,  B-TBC-,  B-TLC-,  B-RBC-,  B-RLC-,  B-RTC-,  B-BLC-,  B-BTC-,  B-BRC-,  B-LTC-,  B-LRC-,  B-LBC-, '  +
            'B-TR-T-,          B-TL-T-, B-RB-T-, B-RL-T-, B-RT-T-, B-BL-T-, B-BT-T-, B-BR-T-, B-LT-T-, B-LR-T-, B-LB-T-, ' +
            'B-TRCT-,          B-TLCT-, B-RBCT-, B-RLCT-, B-RTCT-, B-BLCT-, B-BTCT-, B-BRCT-, B-LTCT-, B-LRCT-, B-LBCT-, ' +
            'B-TR-B-,          B-TL-B-, B-RB-B-, B-RL-B-, B-RT-B-, B-BL-B-, B-BT-B-, B-BR-B-, B-LT-B-, B-LR-B-, B-LB-B-, ' +
            'B-TRCB-,          B-TLCB-, B-RBCB-, B-RLCB-, B-RTCB-, B-BLCB-, B-BTCB-, B-BRCB-, B-LTCB-, B-LRCB-, B-LBCB-, ' +
            'B-C-, B-C--T-, B-C--B-'
        );
        for(let j = 0; j < children.length; ++j) {
            let tagName = children[j].tagName;                              // Name of the tag
            let w = children[j].parentElement.clientWidth - margin * 2;     // SVG width
            let h = children[j].clientHeight;                               // SVG height
            let s = '';                                                     // Output path string
            let f = tagName.charCodeAt(2);                                  // First position (From)
            let t = tagName.charCodeAt(3);                                  // Second potision (To)
            let m = 0; switch(tagName[5]) { case 'T': m = -1; break; case 'B': m = 1; }; //! Y is inverted
            children[j].className += "syntax-arrow";                        // ^ Height center location (1/0/-1 for top/center/bottom)


            // Fix vertical height in non connecting arrows
            if((tagName[3] == 'T' || tagName[3] == 'B') && tagName[4] != 'C'){
                h -= margin * 2;
                children[j].style.padding = "var(--syntax-arrow-margin) 0 var(--syntax-arrow-margin) 0";
            }


            // Connector
            if(tagName[2] == 'C') {
                s =
                    `M${ this.toDecimalPrecision(w * 1, 4) },${ this.toDecimalPrecision((0.5 + mfix / 2 * m) * h, 4) }` +
                    `l${ this.toDecimalPrecision(margin * 2, 4) },0`
                ;
            }


            // Arrows
            else {
                // Calculate coordinates
                let ma = (m && (tagName[2] == 'R' || tagName[2] == 'L')) ? (mfix * m) : 0;
                let mb = (m && (tagName[3] == 'R' || tagName[3] == 'L')) ? (mfix * m) : 0;

                let
                    ax = (Math.cos(a[f]) * (1    ))      / 2 + 0.5,   // First  segment - x of beginning
                    aX = (Math.cos(a[f]) * (1 - r))      / 2 + 0.5,   // First  segment - x of end
                    ay = (Math.sin(a[f]) * (1    ) + ma) / 2 + 0.5,   // First  segment - y of beginning
                    aY = (Math.sin(a[f]) * (1 - r) + ma) / 2 + 0.5,   // First  segment - y of end
                    bx = (Math.cos(a[t]) * (1    ))      / 2 + 0.5,   // Second segment - x of beginning
                    bX = (Math.cos(a[t]) * (1 - r))      / 2 + 0.5,   // Second segment - x of end
                    by = (Math.sin(a[t]) * (1    ) + mb) / 2 + 0.5,   // Second segment - y of beginning
                    bY = (Math.sin(a[t]) * (1 - r) + mb) / 2 + 0.5,   // Second segment - y of end

                    cx = 0 / 2 + 0.5,                                       // Curve center - x
                    cy = (ma ? ma : mb) / 2 + 0.5,                          // Curve center - y

                    h0x = Math.cos(a[t] + Math.PI * 1.25),                  // Head - x of left  segment
                    h0y = Math.sin(a[t] + Math.PI * 1.25),                  // Head - y of left  segment
                    h1x = Math.cos(a[t] - Math.PI * 1.25),                  // Head - x of right segment
                    h1y = Math.sin(a[t] - Math.PI * 1.25)                   // Head - y of right segment
                ;


                // Draw arrow
                s =
                    `M${ this.toDecimalPrecision(ax * w, 4) },${ this.toDecimalPrecision(ay * h, 4) }` +                // Starting position
                    `L${ this.toDecimalPrecision(aX * w, 4) },${ this.toDecimalPrecision(aY * h, 4) }` +                // Draw starting segment
                    `Q${ this.toDecimalPrecision(cx * w, 4) },${ this.toDecimalPrecision(cy * h, 4) }` +                // Draw first half of curve
                    ` ${ this.toDecimalPrecision(bX * w, 4) },${ this.toDecimalPrecision(bY * h, 4) }` +                // Draw second hard of curve
                    `L${ this.toDecimalPrecision(bx * w, 4) },${ this.toDecimalPrecision(by * h, 4) }`                  // Draw ending segment
                ;

                // Draw head
                if(tagName[4] != 'C') s +=
                    `m${ this.toDecimalPrecision(h0x * arrow_d, 4) },${ this.toDecimalPrecision(h0y * arrow_d, 4) }` +  // Starting position
                    `L${ this.toDecimalPrecision(bx * w,        4) },${ this.toDecimalPrecision(by * h,        4) }` +  // Draw head form 0 to O
                    `l${ this.toDecimalPrecision(h1x * arrow_d, 4) },${ this.toDecimalPrecision(h1y * arrow_d, 4) }`    // Draw head from O to 1
                ;
            }


            // Add HTML boilerplate
            children[j].innerHTML =
                `<svg width="${ w }" height="${ h }">` +
                    `<path d="${ s }"style="stroke: var(--syntax-fg-arrow); stroke-width: 2px; fill: none;"/>` +
                `</svg>`
            ;
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
    format_code : function() {
        let c = document.querySelectorAll('example2-');
        for(let i = 0; i < c.length; i++){
            let divs = c[i].querySelectorAll('div');
            for(let j = 0; j < divs.length; ++j){
                divs[j].innerHTML = setup_syntax.format(divs[j].innerHTML) + '<br>';
                // Add trailing newline to fix CSS's bugged max-height  ^
            }
        }
    },







    init : function() {
        setup_syntax.even_widths();
        setup_syntax.format_arrows();
        setup_syntax.format_code();
    }
}