# Talpunkter — The Trustless Thing

*Lösa hållpunkter att tala utifrån — inte ett manus att läsa innantill. Tonen är medvetet pratig. Punkterna under **Bra att ha redo** är detaljer ur uppsatsen som inte står på bilden — bra att kunna dra eller ha i bakfickan om någon frågar.*

---

## Bild 1 — Titelbild
### The Trustless Thing

- Hej allihop — jag heter Tord, och det här är mitt kandidatarbete: kan en publik blockkedja faktiskt fungera som backend åt en IoT-enhet?
- Titeln är en ordlek: "thing" som i Internet of Things, och "trustless" — tillitslös — för att hela poängen med en publik kedja är att man inte behöver lita på någon central part.
- Jag tänkte ta er genom det i sju steg: inledning, bakgrund, teori, metod, resultat, diskussion och slutsatser. Säg gärna till om något är oklart längs vägen.

En fotnot är att denna presentation transkriberades efter önskemål för ett par timmar sedan till svenska, så fråga gärna om ordvalet känns märkligt.
---

## Bild 2 — Avsnittsdelare: Del 1, Inledning

- Det hela började med att jag blev övertygad att förlora en massa pengar på att investera i blockkedjor, vilket fick mig att fundera kring huruvide det faktiskt går att använda en blockkedja till något.

---

## Bild 3 — Inledning: Motivation
### Motivet: hitta ett riktigt användningsfall

- Utgångspunkten var nästan naiv: det finns hundratals publika blockkedjor som lovar guld och gröna skogar — men vad kan man faktiskt bygga med dem, bortom en token eller en NFT?
- Så jag valde med flit något tråkigt och jordnära — en jordfuktighetssensor — och ställde frågan: kan den skriva sina mätvärden direkt till kedjan, helt utan en server emellan?
- Siffrorna till höger sätter ramen — varför man ens bryr sig om IoT-säkerhet: **22 %** av organisationerna fick en driftstörande IoT-incident det senaste året, en genomsnittlig hemenhet möter ett tiotal attacker per dygn, och DDoS väntades öka kraftigt under 2024.

> **Bra att ha redo** (finns i uppsatsen, syns inte på bilden)
> - Centraliserade IoT-arkitekturer skapar en "single point of failure" — om molnservern går ner eller blir kapad kan hela nätverket slås ut (uppsatsens motivering, kap. 1.1).
> - Idén är inte ny: flera forskare har föreslagit blockkedjor som säkerhetsmekanism för IoT — men det jag tillför är ett faktiskt körande, billigt bygge i stället för enbart teori.
> - Om någon frågar varför just nu: poängen är att hundratals *publika* kedjor numera stödjer tekniken som krävs, vilket inte var fallet för några år sedan.

---

## Bild 4 — Avsnittsdelare: Del 2, Bakgrund

> *Delare. "Innan vi går in på lösningen — lite bakgrund, och själva problemet jag försöker lösa."*

---

## Bild 5 — Bakgrund: Problemet
### En enda central nod som alla hänger på
- Så här ser en typisk IoT-uppställning ut idag: alla enheter pratar med ett centralt moln som godkänner, lagrar, nekar och styr. Bekvämt — men det betyder att allt hänger på den där enda noden.
- Tre problem faller ut ur det: slår man ut den centrala noden dör hela systemet; data kan ändras i tysthet eftersom man måste lita på operatören; och en central ändpunkt är precis vad en DDoS-angripare letar efter.
- Notera ordet "ofta" i rubriken — jag påstår inte att ALLA IoT-system ser ut så, men den centraliserade klient–server-modellen är det vanligaste mönstret.
- Centralt betyder inte att det sitter en linuxburk i Skövde, utan systemen är oftast redan skalbara och distribuerade, men ägandeskaped är centraliserat.

> **Bra att ha redo**
> - I bakgrunden delas IoT upp i fyra lager (Application, Endpoint/databehandling, Network, Client/sensing) — min prototyp mappar sen exakt mot de lagren (Tabell 2.1 och 4.1).
> - "Cloud Service" i uppsatsen betyder brett: en privat server, Azure IoT Hub, en Google Cloud-instans — alltså vilken central part som helst som äger lagring och beslut.
> - Säkerhetsproblemet handlar i grunden om autentisering och åtkomstkontroll, som idag nästan alltid kräver tillit till en central auktoritet.

---

## Bild 6 — Bakgrund: Idén & frågeställningar
### Tänk om enheten pratar direkt med kedjan?

- Idén är att ta bort den skräddarsydda backenden helt. I stället är blockkedjan plus ett smart kontrakt backenden.
- Det blir tre roller: klienten (ESP32-sensorn) signerar och skickar sina egna avläsningar; det smarta kontraktet lagrar dem med en tidsstämpel; och en mottagare — t.ex. en iOS-app — läser tillbaka dem via RPC.
- Och så de tre forskningsfrågorna som styr hela arbetet:
  - **RQ1** — vilka tekniska utmaningar måste lösas?
  - **RQ2** — vilka fördelar och begränsningar följer?
  - **RQ3** — hur väl avhjälper min prototyp dem i praktiken?

> **Bra att ha redo**
> - "RPC-nod" är nyckelordet att kunna förklara: det är den hostade tjänst som tar emot enhetens JSON-anrop och vidarebefordrar det till kedjan — i princip länken mellan enhet och blockkedja.
> - Jag byggde faktiskt ett eget ramverk (r2web3) för EVM-interaktion för att få det här att gå på en mikrokontroller — bra att nämna att det inte fanns färdigt.
> - Mottagaren sköter också provisionering och säkerhet UTAN att man behöver dela den privata nyckeln.

---

## Bild 7 — Avsnittsdelare: Del 3, Teori

> *Delare. "Nu till teorin — vad litteraturen säger att man vinner, vad man förlorar, och varför man inte kan få allt."*

Ambitionen har varit att kritiskt undersöka de aspekter av integrationen som jag kunnat identifiera. 

---

## Bild 8 — Teori: Fördelarna
### Vad en publik kedja faktiskt ger dig

- Det här är sex egenskaper man får mer eller mindre gratis. Jag rabblar inte alla — jag lyfter de tyngsta och låter resten stå.
- De viktigaste: **identitet** (varje enhet äger en privat nyckel, nästan omöjlig att förfalska), **motståndskraft** (peer-to-peer, inget centralt mål för DDoS) och **låg underhållskostnad** (inga servrar att driva — nätverkets noder ÄR infrastrukturen).
- De andra tre — anonymitet/censurmotstånd, oföränderlighet och ett standardiserat EVM-gränssnitt — hänger ihop med samma grundtanke: data sprids globalt och alla EVM-kedjor talar samma språk.

> **Bra att ha redo**
> - Identitet motverkar konkret "node cloning" — en känd attackvektor där en falsk enhet utger sig för en riktig; med unik nyckel per enhet blir det kryptografiskt omöjligt.
> - Oföränderlighet gäller specifikt EVM-kedjor — värt att veta att t.ex. Solana TILLÅTER uppdatering av driftsatta program, så det är inte en universell blockkedjeegenskap.
> - Standardisering pekas i forskningen ut som en lösning på IoT:s interoperabilitetsproblem — alla EVM-kedjor delar samma protokoll.

---

## Bild 9 — Teori: Haken
### ...och priset man betalar

- För varje fördel finns en baksida. Det här är inte finstilt jag gömmer — det är halva poängen med arbetet.
- De tre som gör mest ont i praktiken: **konfidentialitet** (kedjan är publik — vem som helst med adressen läser allt), **genomströmning** (bekräftelser är långsamma och ojämna) och **transaktionskostnad** (varje skrivning kostar token, och tokenpriser svänger vilt).
- Plus tre till: omogenhet (kedjor kraschar fortfarande — IOTA, Terra, FTX), uppgraderbarhet (driftsatt kontrakt är hugget i sten) och regelverk/GDPR (permanent data krockar med rätten att bli bortglömd, och det finns ingen ansvarig part).

> **Bra att ha redo**
> - GDPR-poängen är den skarpaste: "rätten att bli bortglömd" är i princip omöjlig på en oföränderlig kedja — därför olämpligt för konsumentdata med personuppgifter.
> - Ingen SLA: vanliga molnleverantörer (Amazon, Google, Microsoft) ger ett serviceavtal med kompensation vid driftstopp — en publik kedja har ingen sådan motpart.
> - Konfidentialitet kan delvis lösas med kryptering, men det ökar komplexiteten — och risk för att exponera mönster i transaktioner kvarstår.

---

## Bild 10 — Teori: Trilemmat
### Varför man inte kan få allt

- Det här binder ihop hela teoridelen. Blockkedjans trilemma: säkerhet, decentralisering, skalbarhet — du kan optimera två, aldrig alla tre samtidigt.
- Premierar du ett hörn betalar du i ett annat. Och det dyker upp överallt i mitt arbete som konkreta avvägningar.
- Till höger: låg underhållskostnad står mot transaktionskostnad; oföränderlig kod står mot uppgraderbarhet; motståndskraft och anonymitet står mot konfidentialitet; säkerhet och decentralisering står mot genomströmning. Varje designbeslut är ett val på den här skalan.

> **Bra att ha redo**
> - Den nedsatta hastigheten ses i forskningen ofta som ett MEDVETET pris för decentralisering och motståndskraft — inte en bugg, utan en avvägning.
> - Bra övergångsmening till metoden: "Min prototyp är i grunden en serie sådana här val — nu visar jag hur jag gjorde dem."

---

## Bild 11 — Avsnittsdelare: Del 4, Metod

> *Delare. "Då går vi från teori till praktik — vad jag faktiskt byggde och mätte."*

---

## Bild 12 — Metod: Prototypen
### En jordfuktighetssensor som lever på kedjan

- Hårdvaran är medvetet billig: en ESP32 plus en DHT11, runt fem dollar. Poängen är att visa att det INTE krävs något exotiskt.
- Enheten skapar sitt eget nyckelpar vid första uppstarten och sparar det i EEPROM — det är dess identitet, som ett eget "bankkonto". En iOS-app provisionerar den över Bluetooth: skickar Wi-Fi-uppgifter och lite medel, och kopplar sen från.
- Två knep för att hålla nere kostnad: jag packar fem 16-bitars avläsningar i ETT 256-bitars heltal per transaktion, och enheten sover mellan mätningarna — vaknar, signerar, sänder, somnar. Sparar både pengar och batteri.

> **Bra att ha redo**
> - Nyckeln genereras med secp256k1-kurvan via ECDSA — samma kryptografi som Ethereum använder för att signera transaktioner.
> - Ärlig brasklapp om säkerhet: i prototypen lagras nyckeln OKRYPTERAD i EEPROM och BLE-säkerhetsfunktionerna används inte — i skarp drift skulle man lägga nyckeln i en HSM eller ett säkert chip.
> - Jag testade RPC-anropen även på en svagare ESP8266 för att se hur det funkar på mer resursbegränsad hårdvara (mätsiffror finns på resultatsidan).
> - Mätdata kunde inte sparas på mikrokontrollern, så den reläades till en Python-server som loggade allt i en SQLite-databas.

---

## Bild 13 — Metod: Experimentet
### Samma anrop, fem kedjor, två tidpunkter

- Upplägget är enkelt med flit: samma anrop — storeValue — körs var 30:e minut mot fem EVM-kompatibla kedjor samtidigt (typ; Shimmer ena gången och Immutable X andra): Avalanche, Polygon, IoTeX, ShimmerEVM och Immutable X.
- För varje transaktion läser jag av "kvittot" — bekräftelsetid, kostnad, eventuella fel — och loggar till SQLite. Kostnaden räknas sen om till kronor/dollar med historiska tokenpriser från CoinMarketCap.
- Den viktigaste — och ärligaste — detaljen: jag mätte vid **två tillfällen med ~16 månaders mellanrum** (feb 2024 och juni 2025), 500 transaktioner per kedja. Det är guld för att se hur snabbt ekosystemet utvecklas, MEN förhållandena var inte identiska — och det måste man ha i bakhuvudet när man tolkar resultaten.

> **Bra att ha redo**
> - Varför just de fem: Avalanche och Polygon för att de är stora (marknadsvärde); ShimmerEVM och IoTeX för deras IoT-fokus; Immutable X för låga avgifter (gaming).
> - Immutable X var egentligen ett SENT tillägg — ShimmerEVM:s prisdata försvann mitt under mätningen, så jag tog in Immutable X som ersättande referens. Bra att kunna förklara om någon undrar varför ShimmerEVM saknar 2025-data.
> - Avalanche-plånboken tog SLUT halvvägs genom 2024-mätningen (höga avgifter då), så totalsumman för Avalanche 2024 är interpolerad — därav parentesen runt ~18 $.
> - Jag justerade aldrig gas-gränsen för trängsel (använde standardvärdet 6 721 975) och hade ingen logik för priofees — det förklarar en del av felen senare.
> - Ethereum mainnet uteslöts med flit — kostnaderna där är flera tiopotenser högre.

---

## Bild 14 — Avsnittsdelare: Del 5, Resultat

> *Delare. "Så — vad blev det? Två saker: kostnad och prestanda."*

---

## Bild 15 — Resultat: Kostnad
### Kostnaden rasade på 16 månader

- Den stora rubriken: kostnaden för att skriva till kedjan föll dramatiskt. Avalanche 2025 låg på ungefär en tjugondel — runt 5 % — av 2024 års kostnad, prisjusterat.
- I praktiken: under ett öre per transaktion under 2025. Billigast var Polygon och Immutable X.
- Men — och det här är viktigt att säga själv innan någon påpekar det: totalsummorna är **till stor del en marknadseffekt**. Det är tokenpriserna som rört sig, inte nödvändigtvis att protokollen blivit "bättre". Jämför man rättvist måste man prisjustera.

> **Bra att ha redo**
> - Konkreta snittsiffror om någon vill ha dem: Avalanche 0,035 $ (2024) → 0,0014 $ (2025); IoTeX 0,0075 → 0,0016 $; Polygon 0,002 → 0,00035 $; Immutable X ~0,0003 $ (2025).
> - "Prisjusterat" betyder: 2025 års transaktionsmängd räknad MOT 2024 års tokenpris — det isolerar protokollets förbättring från marknadssvängningen.
> - Två konkreta sätt att sänka kostnaden ytterligare (från diskussionen): använd Event Logs i stället för SSTORE-opcoden, eller batcha flera mätningar i en transaktion.

---

## Bild 16 — Resultat: Prestanda
### Hastighet och tillförlitlighet spretade rejält

- Här är bilden mer rörig — och ärligt talad. Felfrekvensen gick från noll (Immutable X) ända upp till nära en tredjedel — 29 % — av skrivningarna som misslyckades på Avalanche 2025.
- Vad var det som fallerade? Mest "nonce too low" och "no receipt" — i klartext: mitt SDK **återanvände ett nonce** medan en långsam transaktion fortfarande hängde. Orsaken var alltså trängsel plus en naiv nonce-strategi i MIN kod — inte ett grundfel i kedjorna.
- Det är en viktig nyans: jag skyller inte på blockkedjorna. Felen säger lika mycket om min implementation som om näten.

> **Bra att ha redo**
> - Transaktionstiderna konkret: 2025 låg Polygon ~4,8 s, Immutable X ~4,0 s, IoTeX ~5,4 s — men Avalanche sköt iväg till ~160 s (från ~2,5 s 2024). De långa tiderna och de höga felen hänger ihop.
> - Beräkningstid på enheten (computational overhead): ESP32 kodade en transaktion på 97 ms, den svagare ESP8266 på 601 ms — nättransfer ~1 s. Alltså: själva signeringen är inte flaskhalsen, väntan på kedjan är det.
> - Möjlig fix för trängsel: höj priofee (tip) dynamiskt, eller "fire-and-forget" för anrop som inte kräver bekräftelse.
> - Transaktionstiden är dessutom en grov uppskattning — validatorn sätter blockets tidsstämpel, så den är aldrig exakt.

---

## Bild 17 — Avsnittsdelare: Del 6, Diskussion

> *Delare. "Vad betyder det här? Levde prototypen upp till teorin — och vad kan resultaten faktiskt visa?"*

Nu presenteras vad som faktiskt mättes (och inte mättes).
---

## Bild 18 — Diskussion: Levererade prototypen?
### De flesta förväntade fördelarna höll

- Om jag betygsätter prototypen mot teorin: det mesta blev HÖG. Autentisering, oföränderlighet, underhållskostnad och säkerhet — alla höll i praktiken.
- Två blev MEDEL: anonymitet/censurmotstånd (håller så länge RPC-noderna finns, men avanonymisering är möjlig) och standardisering (EVM är strikt, men spretiga felformat avslöjar att kedjorna implementerar olika).
- Och en blev BEGRÄNSAD — den ärliga svagheten: **motståndskraft**. Hela systemet hänger på en enda RPC-nod från tredje part. Slår man ut den är enheten avskuren, hur decentraliserad kedjan än är bakom.

> **Bra att ha redo**
> - Det här är paradoxen värd att lyfta: kedjan är decentraliserad, men min ÅTKOMST till den går genom en central RPC-nod — så jag återinför en "single point of failure" i åtkomstledet.
> - Lösningar jag diskuterar: en fallback-RPC-nod, eller en egen self-hosted nod med IP-whitelisting. Men — det finns vad jag vet ingen systematisk standardlösning för RPC-nodfel ännu.
> - De inkonsekventa felformaten (standardisering=MEDEL) gjorde felsökning svår — det är en konkret praktisk lärdom, inte bara teori.

---

## Bild 19 — Diskussion: Var det lämpar sig
### Ärligt: fel verktyg för de flesta projekt

- Jag vill vara tydlig — det här är INTE en universell IoT-backend. Det passar en smal nisch, och presterar dåligt utanför den.
- Lämpligt när: datavolymerna är låga och sällsynta, sekunder duger (inte millisekunder), data inte är privat, och där äkthet och decentralisering verkligen betyder något — med minimal juridisk/GDPR-exponering.
- Olämpligt när: du hanterar personuppgifter, behöver hög genomströmning eller realtid, måste kunna radera data (GDPR), vill kunna ändra appen lätt, eller behöver ett SLA och en ansvarig motpart.
- Det här är egentligen svaret på "så vad ska jag ta med mig?" — och jag säger det hellre rakt ut än låter det låta bättre än det är.

---

## Bild 20 — Diskussion: Metodens begränsningar
### Vad resultaten kan — och inte kan — visa

- Här är jag självkritisk, för det hör till. Fyra saker att vara ärlig om.
- Ett: jag mätte egentligen bara **tillgänglighet** direkt. Programvarubegränsningarna delvis, de juridiska inte alls — så stora delar av teorin är obekräftad av experimentet.
- Två: ingen kontrollerad miljö — jag körde på aktiva publika nät, där trängsel, mempool-beteende och gas-svängningar är okontrollerade variabler. Tre: bara två ögonblicksbilder, 16 månader isär, utan upprepade försök — verifierbart men nästan omöjligt att replikera.
- Fyra: jag rangordnar INTE kedjorna. Problemen kan vara tillfälliga eller bero på min konfiguration — avsikten var att illustrera skillnader, inte kröna en vinnare.

> **Bra att ha redo**
> - Distinktionen att kunna: resultaten går att VERIFIERA (transaktionsdatabaserna finns i appendix) men är svåra att REPLIKERA (nätens villkor ändras hela tiden).
> - Ett kontrollerat, emulerat nät hade kunnat skilja på om en fördröjning beror på MIN design eller på kedjans protokoll — det är den största validitetsbristen.
> - Kostnadsmätningen kan vara skev eftersom tokenpriser svänger även inom ett dygn.

---

## Bild 21 — Diskussion: Omfattning & brasklapp
### Mycket teori kartlagd — lite mätt

- Den här bilden ramar in hela ärlighetspoängen. Jag delar in varje fördel och begränsning i tre kategorier — och bara en av dem gick att mäta meningsfullt.
- **Tillgänglighet** (åtkomst, hastighet, omogenhet, kostnad) — den **MÄTTES**. Programvara (uppgraderbarhet, beräkning, lagring) — **DELVIS**. Och juridik (ansvar, konfidentialitet, reglering, datapersistens) — bara **DISKUTERAD**.
- Varför ta upp det som inte mättes alls? För att jag tyckte de juridiska bitarna var för viktiga för att tiga om — flera av dem har faktiskt tekniska lösningar, så de hör hemma i bilden även om experimentet inte rörde dem.

> **Bra att ha redo**
> - Den här bilden ligger sent i decket men hör tankemässigt ihop med teorin (de tre kategorierna). En övergång som funkar: "Minns ni de tre kategorierna? Så här långt nådde experimentet faktiskt."
> - Juridik har tekniska motsvarigheter: kryptering med läs-nyckel (lagrad i HSM) för konfidentialitet, off-chain-lagring med bara en hash på kedjan för GDPR/datapersistens.
> - Om någon trycker på GDPR: off-chain-lagring där kontraktet bara håller en referens är den mest framkomliga vägen — men för konsumentdata kan en publik kedja ändå vara olämplig.

---

## Bild 22 — Avsnittsdelare: Del 7, Slutsatser

> *Sista delaren. "Och för att knyta ihop det — slutsatserna."*

---

## Bild 23 — Slutsatser
### Genomförbart — men för en nisch

- Fyra hållpunkter, och så är jag klar. Ett: det ÄR genomförbart. En enhet för några dollar kan signera och publicera till en publik kedja, utan egen backend, med ett granskningsbart spår.
- Två: det förbättras snabbt. Kostnaderna rasade på 16 månader och tillförlitligheten blev bättre — ekosystemet rör sig framåt.
- Tre: men det är en **nisch**. Bäst för låg volym, låg integritet och utan tidskrav, där äkthet och decentralisering är det som räknas.
- Fyra: designa därefter — planera för flera kedjor, kryptera eller lagra känslig data off-chain, och använd ett registerkontrakt för att hantera uppgraderingar. Tack så mycket — jag tar gärna frågor.

> **Bra att ha redo**
> - Huvudbudskapet i en mening: publika blockkedjor är inte en universell IoT-backend, men de KAN passa bra där enkel backend, dataäkthet och decentralisering väger tyngre än fart och integritet.
> - "Registerkontrakt" (registry contract) = ett litet meta-kontrakt som pekar ut adressen till det riktiga kontraktet, så man kan byta ut det senare — lösningen på uppgraderbarhetsproblemet.
> - Framtidsspaning: i takt med att konsensusalgoritmer blir effektivare lär kostnaderna fortsätta falla — och multi-chain-design rekommenderas för att inte låsa sig vid en enda kedja.
> - Koden och transaktionsdatabaserna ligger på GitHub (github.com/TordWessman/BachelorThesis) om någon vill granska.

Personligen, så anser jag att omogenheten är det största problemet. Ett system som inte går att lita på och som saknar ansvarsutkrävande är i praktiken omöjligt att förlita sig på.
---

## Tips för framförandet

- Delarbilderna (2, 4, 7, 11, 14, 17, 22) är andningspauser — säg en mening och gå vidare, stanna inte.
- Den röda tråden genom hela snacket är ärlighet: du säljer inte tekniken, du visar var den funkar och var den inte gör det. Det är din styrka — luta dig mot den.
- Om tiden tryter: korta ner Fördelarna (bild 8) och Haken (bild 9) — det räcker att nämna tre av sex på var. Spara tid till resultaten och diskussionen, där dina egna mätningar finns.
