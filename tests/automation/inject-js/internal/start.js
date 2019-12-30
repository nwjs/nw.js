var result = result ||[];
result.push('inject-js-start');

window.onload = function(){
	result.push('onload')
};