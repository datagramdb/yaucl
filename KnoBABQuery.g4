grammar KnoBABQuery;

queries : load_data_query
        | display_data
        | model_query
        | query_plan
        | set_benchmarking_file
        ;

set_benchmarking_file: 'benchmarking-log' file=STRING;
load_data_query: 'load' (HRF|TAB|XES|log) file=STRING with_data? no_stats? with_missing? 'as' env_name=STRING ;
display_data: 'display' (ACT_TABLE|CNT_TABLE|(ATT_TABLE attr=STRING)) 'for' STRING #display
            | 'list'    (ATT|ACTIVITYLABEL|LOGS)                                         'for' STRING #list
            | 'droplog' env_name=STRING #droplog
            ;

log : 'log' LPAREN (trace ';')* trace RPAREN ;
trace: data_part ':' event*;
event : LABEL data_part?;
data_part: '{' field* '}';
field : var '=' (NUMBER | STRING) ;

model_query: 'model-check' model
             'using' ensemble=STRING
             ('plan' plan=STRING)
             ('with' 'operators' operators=STRING)?
             ('multithreaded' nothreads=INTNUMBER)?
             display_qp?
             atomization?
             grounding?
           ;

model: 'file' STRING                          #file_model
     | 'declare' data_aware_declare           #declares
     | 'template' STRING 'logtop' INTNUMBER   #topn
     ;
atomization: 'atomize!' ('with' 'label' label=STRING)?
                       ('maximum' 'strlen' strlen=INTNUMBER)?
                       ('with' 'strategy' strategy=STRING)?
           ;
grounding: 'grounding!' ('with' 'strategy' strategy=STRING)?
                       no_preliminary_fill?
                       act_for_attributes?
                       no_cream_off?
         ;
display_qp : 'display' 'query-plan';

query_plan : AUTO_TIMED? 'queryplan' STRING LPAREN declare_syntax+ RPAREN;
declare_syntax : 'template' STRING has_args? ':=' ltlf;
has_args : 'args' INTNUMBER;
ltlf : INIT TIMED?                      declare_arguments? declare_act_target?        #init
      | END  TIMED?                      declare_arguments? declare_act_target?        #end
      | LAST                                                declare_act_target?        #last
      | FIRST                                               declare_act_target?        #first
      | EXISTS NEGATED? INTNUMBER TIMED? declare_arguments? declare_act_target?        #exists
      | ABSENCE INTNUMBER TIMED?         declare_arguments? declare_act_target?        #absence
      | NEXT ltlf                                                                     #next
      |<assoc=right> ltlf OR TIMED? THETA? ltlf                                      #or
      |<assoc=right> ltlf AND TIMED? THETA? ltlf                                     #and
      |<assoc=right> ltlf '=>' TIMED? THETA? ltlf                                    #implication
      |<assoc=right> IF TIMED? ltlf THEN ltlf THETA? ELSE ltlf                      #ifte
      |<assoc=right> ltlf UNTIL TIMED? THETA?  ltlf                                  #until
      | BOX TIMED?  ltlf                                                              #box
      | DIAMOND TIMED?   ltlf                                                         #diamond
      | NEGATED TIMED? ltlf PRESERVE?                                                 #not
      | '(' ltlf ')'                                                                  #paren
      |<assoc=right> ltlf '&Ft' THETA? ltlf                                          #and_future
      |<assoc=right> ltlf '&XGt' THETA? ltlf                                         #and_next_globally
      |<assoc=right> ltlf '&Gt' THETA? ltlf                                          #and_globally
      ;

data_aware_declare: (declare)*;

declare: name=STRING '(' (fields ',')+ fields ')' ('where' prop)? #nary_prop
       | name=STRING '(' fields ',' INTNUMBER ')' #unary_prop
       ;

fields: label=STRING ',' prop;

prop  : prop_within_dijunction '||' prop  #disj
      | prop_within_dijunction            #conj_or_atom
      | 'true'                            #top
      ;

prop_within_dijunction : atom                              #in_atom
                       | atom '&&' prop_within_dijunction  #atom_conj
                       ;

atom : (isnegated='~')? var rel (NUMBER | STRING | leftvar=var) ;

rel   : '<' #lt
      | '<=' #leq
      | '>' #gt
      | '>=' #geq
      | '=' #eq
      | '!=' #neq
      ;

declare_arguments : '#' INTNUMBER;
declare_act_target : ACTIVATION | TARGET;
no_preliminary_fill : 'no' 'preliminary' 'fill';
act_for_attributes : 'no' 'act' 'attributes';
no_cream_off: 'no' 'cream' 'off';
with_data: 'with' 'data';
with_missing: 'with' 'missing';
no_stats: 'no' 'stats';

ACT_TABLE: 'ACTTABLE';
CNT_TABLE: 'COUNTTABLE';
ATT_TABLE: 'ATTRIBUTETABLE';
ACTIVITYLABEL: 'ACTIVITYLABEL';
LOGS: 'LOGS';
ATT: 'ATTRIBUTES';
ACTIVATION: 'activation';
TARGET: 'target';
INIT: 'INIT';
END: 'END';
EXISTS: 'EXISTS';
ABSENCE: 'ABSENCE';
NEXT : 'NEXT';
OR : 'OR';
AND: 'AND';
FIRST: 'FIRST';
LAST: 'LAST';
IF : 'IF';
THEN: 'THEN';
ELSE: 'ELSE';
UNTIL: 'U';
BOX: 'G';
DIAMOND : 'F';
AUTO_TIMED: 'auto-timed';
LPAREN : '{';
RPAREN : '}';
PRESERVE: 'PRESERVE';
TIMED: 't';
THETA: 'THETA';
LEFT : 'L';
RIGHT: 'R';
MIDDLE: 'M';
NEGATED: '~';

HRF: 'HRF';
TAB: 'TAB';
XES: 'XES';

var: 'var' STRING;
LABEL: ('A'..'Z')[a-zA-Z]*;
INTNUMBER : ('0'..'9')+ ;
NUMBER : '-'? INTNUMBER ('.' INTNUMBER)?;
STRING : '"' (~[\\"] | '\\' [\\"])* '"';
SPACE : [ \t\r\n]+ -> skip;

COMMENT
    : '/*' .*? '*/' -> skip
;

LINE_COMMENT
    : '//' ~[\r\n]* -> skip
;