jQuery(function () {
    jQuery('#ht_product_tabs').tabs({
        activate: function (e) {
            let active = jQuery(e.originalEvent.currentTarget).parent()
            let parent = jQuery(active.parent()).children()
            for (let i = 0; i < parent.length; i++ ) {
                let item = jQuery(parent[i])
                let activeColor = item.data('active');
                let inactive = item.data('inactive');
                if (activeColor === inactive) {
                    item.css('background', "linear-gradient(135deg, #1ebbf0 30%, #39dfaa 100%)");
                    continue;
                }
                let color = item.data('inactive')
                item.css('background-color', color)
            }
            let activeColor = active.data('active');
            let inactive = active.data('inactive');
            if (activeColor === inactive) {
                active.css('background', "linear-gradient(135deg, #1ebbf0 30%, #39dfaa 100%)");
            } else {
                active.css('background-color', active.data('active'))
            }
        }
    })
    let li = jQuery('#ht_product_tabs>ul').children()
    for (let i = 0; i < li.length; i++ ) {
        let item = jQuery(li[i]);
        let active = item.data('active');
        let inactive = item.data('inactive');
        if (active === inactive) {
            item.css('background', "linear-gradient(135deg, #1ebbf0 30%, #39dfaa 100%)");
            continue;
        }
        let color;
        if (i === 0) {
            color = item.data('active');
        } else {
            color = item.data('inactive');
        }
        item.css('background-color', color)
    }
})