[%bs.raw {|require("./SideBar.css")|}];
[%bs.raw {|require("./Map.css")|}];

[@bs.val] external fetch: string => Js.Promise.t('a) = "fetch";

type feature = {
  .
  "geometry": {
    .
    "coordinates": array(float),
    "type": string,
  },
  "properties": {
    .
    "city": string,
    "citycode": string,
    "context": string,
    "housenumber": string,
    "id": string,
    "importance": float,
    "label": string,
    "name": string,
    "postcode": string,
    "score": float,
    "street": string,
    "type": string,
    "x": float,
    "y": float,
  },
  "type": string,
};

type state =
  | WaitingForUserSearch
  | LoadingAddress
  | ErrorFetchingAddress
  | NoResults
  | LoadedAddress(array(feature));

type currentFeature = feature;

external featuresToJson: array(feature) => Js.Json.t = "%identity";
external jsonToFeatures: Js.Json.t => array(feature) = "%identity";
external featureToJson: feature => Js.Json.t = "%identity";
external jsonToFeature: Js.Json.t => feature = "%identity";

type option('a) =
  | None
  | Some('a);

[@react.component]
let make = () => {
  let (state, setState) = React.useState(() => WaitingForUserSearch);
  let (query: string, setQuery) = React.useState(() => "");

  let extractCoordinatesFromFeature = feature => [|
    feature##geometry##coordinates[1],
    feature##geometry##coordinates[0],
  |];

  let (currentFeature, setCurrentFeature) =
    React.useState(() =>
      {
        "geometry": {
          "coordinates": [|3.01809, 50.632787|],
          "type": "Point",
        },
        "properties": {
          "city": "Lille",
          "citycode": "59350",
          "context": "59, Nord, Hauts-de-France",
          "housenumber": "6",
          "id": "59350_9677_00006",
          "importance": 0.5975467602277059,
          "label": "6 Avenue des Saules 59160 Lille",
          "name": "6 Avenue des Saules",
          "postcode": "59160",
          "score": 0.8725042509297914,
          "street": "Avenue des Saules",
          "type": "housenumber",
          "x": 701282.05,
          "y": 7059534.98,
        },
        "type": "Feature",
      }
    );

  React.useEffect0(() => {
    let localStorageCurrentFeature =
      LocalStorage.getItem("currentFeature")->Js.Nullable.toOption;
    let features = LocalStorage.getItem("features")->Js.Nullable.toOption;
    switch (localStorageCurrentFeature) {
    | None => ()
    | Some(localStorageCurrentFeature) =>
      setCurrentFeature(_previousCurrentFeature =>
        localStorageCurrentFeature->Js.Json.parseExn->jsonToFeature
      )
    };
    switch (features) {
    | None => ()
    | Some(features) =>
      setState(_previousState =>
        LoadedAddress(features->Js.Json.parseExn->jsonToFeatures)
      )
    };

    None;
  });

  let transformSpacesToPlusInQuery = (string: string) => {
    let transformedQuery = ref("");
    for (i in 0 to Js.String.length(string) - 1) {
      transformedQuery :=
        transformedQuery^
        ++ (
          switch (string.[i]) {
          | ' ' => "+"
          | _ => String.make(1, string.[i])
          }
        );
    };
    transformedQuery^;
  };

  let fetchAddress = _event =>
    if (query !== "") {
      setState(_previousState => LoadingAddress);
      Js.Promise.(
        fetch(
          "https://api-adresse.data.gouv.fr/search/?q="
          ++ query->transformSpacesToPlusInQuery,
        )
        |> then_(response => response##json())
        |> then_(jsonResponse => {
             Js.log(jsonResponse);
             switch (Js.Array.length(jsonResponse##features)) {
             | 0 => setState(_previousState => NoResults)
             | _ =>
               LocalStorage.setItem(
                 "features",
                 jsonResponse##features->featuresToJson->Js.Json.stringify,
               );
               setState(_previousState =>
                 LoadedAddress(jsonResponse##features)
               );
             };

             Js.Promise.resolve();
           })
        |> catch(_err => {
             Js.log(_err);
             setState(_previousState => ErrorFetchingAddress);
             Js.Promise.resolve();
           })
        |> ignore
      );
    };

  <>
    <div className="SideBar">
      <form
        onSubmit={e => {
          e->ReactEvent.Form.preventDefault;
          fetchAddress();
        }}>
        <input
          value=query
          onChange={event => {setQuery(event->ReactEvent.Form.target##value)}}
        />
        <button type_="submit"> {React.string("Search")} </button>
      </form>
      {switch (state) {
       | WaitingForUserSearch => <p />
       | ErrorFetchingAddress => <p> {React.string("An error occurred!")} </p>
       | LoadingAddress => <p> {React.string("Loading...")} </p>
       | NoResults =>
         <p> {React.string("No match found on gouvernment database")} </p>
       | LoadedAddress(featureCollection) =>
         <>
           <p />
           <div className="markersContainer">
             {featureCollection
              ->Belt.Array.map(feature => {
                  <div key={feature##properties##id} className="markerBox">
                    <button
                      onClick={_e => {
                        setCurrentFeature(_prevCurrentFeature => feature);
                        LocalStorage.setItem(
                          "currentFeature",
                          feature->featureToJson->Js.Json.stringify,
                        );
                      }}
                      className={
                        feature##geometry##coordinates ==
                        currentFeature##geometry##coordinates
                          ? "selected" : ""
                      }>
                      {feature##properties##label->React.string}
                    </button>
                  </div>
                })
              ->React.array}
           </div>
         </>
       }}
    </div>
    <Map center={currentFeature->extractCoordinatesFromFeature}>
      <Marker
        anchor={currentFeature->extractCoordinatesFromFeature}
        payload=1
      />
    </Map>
  </>;
};