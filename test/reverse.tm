{
	instructions = {		
		<q0,$,q1,$,l>,
		<q1,0,q2,$,r>,
		<q1,1,q21,$,r>,
		<q2,$,q3,$,r>,
		<q2,0,q2,0,r>,
		<q2,1,q2,1,r>,
		<q21,$,q31,$,r>,
		<q21,0,q21,0,r>,
		<q21,1,q21,1,r>,
		<q3,$,q4,0,l>,
		<q3,0,q3,0,r>,
		<q3,1,q3,1,r>,
		<q31,$,q41,1,l>,
		<q31,0,q31,0,r>,
		<q31,1,q31,1,r>,
		<q4,$,q5,$,l>,
		<q4,0,q4,0,l>,
		<q4,1,q4,1,l>,
		<q41,$,q51,$,l>,
		<q41,0,q41,0,l>,
		<q41,1,q41,1,l>,
		<q5,$,q6,0,l>,
		<q5,0,q5,0,l>,
		<q5,1,q5,1,l>,
		<q51,$,q6,1,l>,
		<q51,0,q51,0,l>,
		<q51,1,q51,1,l>,
		<q6,$,q7,$,r>,
		<q6,0,q2,$,r>,
		<q6,1,q21,$,r>,
		<q7,0,q7,0,r>,
		<q7,1,q7,1,r>
	},

	alphabet = {
		$,0,1
	},

	memory = {
		$,1,0,1,1,1,0,0,0,0,0,0,0,0,$
	},

	states = {
		q0,q1,q2,q21,q3,q31,q4,q41,q5,q51,q6,q7
	},

	initial_state = {
		q0
	}
}