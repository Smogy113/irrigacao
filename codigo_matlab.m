%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Created by:    Gilvan Soares Borges
% Contact:       gilvan.borges@ifpa.edu.br
% First version: 2026/01/31
% Last modified: 2026/02/03
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear


%% SETTINGS:

% Define the irrigation plan period:
startDate = datetime(2026, 03, 24,'Format','yyyy-MM-dd');
nProfileCycles = 8;

% Define average behavior:
irrFrequencyMean = 2;
irrDurationMean = 30; % in minutes

% Define dispersive behavior:
maxIrrFrequency = 2*irrFrequencyMean;
irrDurationAmplitude = 20; % in minutes

irrDurationRange = [(irrDurationMean - irrDurationAmplitude/2) (irrDurationMean + irrDurationAmplitude/2)];

% Define different irrigation profiles:
irrigationProfile = struct('name'          ,      {'deficit'                        , 'comfort'                        , 'excess' }, ...
                           'probIrrWindowsRange', {[0.0 0.2]                        , [0.4 0.6]                        , [0.8 1.0]}, ...                      % Probability interval for irrigation within a given window.
                           'probOccurrence',      {0.3                              , 0.4                              , 0.3      }, ...                      % Probability of occurrence of each irrigation profile
                           'irrDurationPdf',      {prob.TriangularDistribution.empty, prob.TriangularDistribution.empty, prob.TriangularDistribution.empty}); % PDF of irrigation duration for each profile, all triangular

sumProbOccurrence = sum([irrigationProfile.probOccurrence]);
if (sumProbOccurrence ~= 1)
    for i = 1:length(irrigationProfile)
        irrigationProfile(i).probOccurrence = irrigationProfile(i).probOccurrence/sumProbOccurrence;
    end
end

irrDurationPeak = [irrDurationRange(1) irrDurationMean irrDurationRange(2)]; % Must match the number of profiles
for i = 1:length(irrigationProfile)
    irrigationProfile(i).irrDurationPdf = makedist('Triangular', ...
        'A', irrDurationRange(1), ...
        'B', irrDurationPeak(i) , ...
        'C', irrDurationRange(2));
end

profileDurationRange = [2 5];

% Define safety limits:
% safetyTrigger = 12;
minIrrigationInterval = irrDurationAmplitude/60; % in hours
nAttemptsMax = 10;

% Define formatting parameters:
irrDurationStrWidth = 5;         % Character width allocated for the irrigation duration string (e.g., "  900", for irrDurationStrWidth = 5)
irrEventColWidth = 17;           % Character width for a single irrigation event entry (e.g., "| HH:mm:ss;  900 ", where irrEventColWidth = 17)


%% PROCESSING:

profileCycles = randsample(1:length(irrigationProfile), nProfileCycles, true, [irrigationProfile(1:length(irrigationProfile)).probOccurrence]);
daysPerProfileCycle = randi(profileDurationRange, 1, nProfileCycles);

% File opening and header:
fileID = fopen('Irrigation_Plan.txt', 'w');
fprintf(fileID, '# =============================================================================\n');
fprintf(fileID, '# RANDOM IRRIGATION PLAN (from %s to %s)\n', [string(startDate), string(startDate + days(sum(daysPerProfileCycle)))]);
fprintf(fileID, '# \n');
fprintf(fileID, '# Format: Data | Hora; Duração | Hora; Duração |... | # Profile\n');
fprintf(fileID, '# Units: Data (aaaa-mm-dd); Horário (HH:mm:ss); Duração (Seconds)\n');
fprintf(fileID, '# \n');
fprintf(fileID, '# %s\n', string(datetime('now')));
fprintf(fileID, '# =============================================================================\n\n');

% Generate daily irrigation schedules based on the selected profile cycles:
currentDate = startDate - days(1);
partitionDay = linspace(0, 24, maxIrrFrequency + 1);
for i = 1:nProfileCycles
    for j = 1:daysPerProfileCycle(i)
        currentDate = currentDate + days(1);
        
        attempts = 1;
        isIrrProximity = true;
        while isIrrProximity  && (attempts <= nAttemptsMax) % Guarantees a minimum time interval between contiguous irrigation events
            relativePos = rand(1, maxIrrFrequency);
            irrStartPositionPerDay = partitionDay + (24/maxIrrFrequency)*[relativePos relativePos(1)];

            isIrrProximity = any(diff(irrStartPositionPerDay) <= minIrrigationInterval);
            attempts = attempts + 1;
        end
        irrStartPositionPerDay = irrStartPositionPerDay(1:end-1); % Irrigation start times within the day, expressed in hours since midnight

        activeWindows = rand(1, maxIrrFrequency) ...
            < (irrigationProfile(profileCycles(i)).probIrrWindowsRange(1) ...
            + (irrigationProfile(profileCycles(i)).probIrrWindowsRange(2) - irrigationProfile(profileCycles(i)).probIrrWindowsRange(1))*rand());
        irrDurationPerWindows = round(activeWindows.*random(irrigationProfile(profileCycles(i)).irrDurationPdf, 1, maxIrrFrequency)*60); % Irrigation duration for each window (in seconds)

        strIrrPlanPerDay = [string(duration(irrStartPositionPerDay, 0, 0)) ; pad(string(irrDurationPerWindows), irrDurationStrWidth, 'left', ' ')];
        strIrrPlanPerDay(:, strIrrPlanPerDay(2, :) == pad("0", irrDurationStrWidth, 'left')) = []; % Remove irrigation windows with zero duration (inactive)
        strIrrPlanPerDay = pad(join([string(currentDate) join(strIrrPlanPerDay.', ";").'], " | ") + " | ",  irrEventColWidth*(maxIrrFrequency+1)) ...
            + "# " + irrigationProfile(profileCycles(i)).name;

        fprintf(fileID, '%s\n', strIrrPlanPerDay);
    end
    fprintf(fileID, '\n');
end
fclose(fileID);

