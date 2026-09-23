'use strict';
const search = document.querySelector('#example-search');
const category = document.querySelector('#example-category');
if (search && category) {
  const rows = [...document.querySelectorAll('#examples-table tbody tr')];
  const count = document.querySelector('#example-count');
  const empty = document.querySelector('#no-examples');
  function filterExamples() {
    const query = search.value.trim().toLowerCase();
    let visible = 0;
    for (const row of rows) {
      const match = row.textContent.toLowerCase().includes(query)
        && (!category.value || row.dataset.category === category.value);
      row.hidden = !match;
      if (match) visible++;
    }
    count.textContent = `${visible} of ${rows.length} examples`;
    empty.hidden = visible !== 0;
  }
  search.addEventListener('input', filterExamples);
  category.addEventListener('change', filterExamples);
}
