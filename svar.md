1. Du byggde ett eget framework r2web3, Är det något andra utvecklare kan använda eller är det bara för det här projektet?
> Ja, absolut. Det är gammalt, men om någon vill ta vid, så är det bara att sloppa fram det till det senaste, heta

2. Du säger att blockchain passar för nischade IoT-applikationer. Kan du ge ett konkret verkligt exempel?
> Diskuterar detta, men t ex väderstationer, seismisk eller hydrologisk amatörövervakning. Vattennivå i en älv, markrörelser i ett rasbenäget område, enkla seismometrar. Medborgarinsamlad data (citizen science). Pollenhalter, vattenkvalitet i en sjö, fågelräkning etc.

3. Du skriver själv i appendix att Avalanches höga felfrekvens berodde på din egen kod, hur kan du då dra slutsatser om Avalanches prestanda?

> Att prestandan var dålig berodde på att jag 
  - valde att kolla efter kvitton. Tog transaktionen lång tid på sig, så blev det inget kvitto -> fel
  - det ledde till ett följdfel. Min transaktion var i själva verket köad och mitt nonce baserades (hämtades) från den senaste noncen. Då fick två transaktioner samma "nummer", varför den senare blev reject:ad. Fallback för att öka nonce hade varit behändigt 

4. Två veckor är en kort mätperiod, tycker du själv att det räcker för att dra trovärdiga slutsatser?
> Nej, en bättre helhetsbild hade givetvis varit en betydligt längre mätperiod (ett år). Anledningen var f a pga praktiska begränsningar. Detta är en svaghet, då empirin blir väldigt bristfällig.

5. Om du fick göra om arbetet, vad hade du gjort annorlunda?
> Längre mätperioder, bättre felhantering. Möjlighet att "tippa" för att kunna hantera congestion för att få mina transaktioner godkända (fallet Avalanche, troligtvis).