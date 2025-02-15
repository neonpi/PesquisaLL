

str = element.no_orgao.normalize('NFD').replace(/[\u0300-\u036f]/g, '');

console.log(str)