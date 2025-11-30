// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.19;

contract MoistureStorage {

    mapping(address => uint256[]) private userValues;
    mapping(address => uint64) private satisfactionValues;

    event Moisture(address indexed device, uint256 value, uint256 timestamp);

    function storeValue(uint256 _value) public {
        //Timestamp issue: if the transaction is dropped for some reason, the next timestamp will not reflect the measurement
        uint256 val = (block.timestamp << (256 - 4 * 8)) | _value;
        userValues[msg.sender].push(val);
        emit Moisture(msg.sender, _value, block.timestamp);
    }

    function storeValueEvent(uint256 _value) public {
        emit Moisture(msg.sender, _value, block.timestamp);
    }

    // Set the level which coresponds to the "satisfaction level" == the values that corresponds to acceptable values
    function setSatisfaction(uint64 _value) public {
        satisfactionValues[msg.sender] = _value;
    }

    function getValues(address _user, uint16 _count) public view returns (uint256[] memory) {
        require(_count > 0, "Count must be greater than zero");
        
        uint256[] memory selectedValues;
        uint256 userValueCount = userValues[_user].length;

        if (_count >= userValueCount) {
            // Return all values if requested count is greater than or equal to the user's total values
            selectedValues = userValues[_user];
        } else {
            // Return the most recent values based on the requested count
            selectedValues = new uint256[](_count);

            for (uint256 i = userValueCount - _count; i < userValueCount; i++) {
                selectedValues[i - (userValueCount - _count)] = userValues[_user][i];
            }
        }

        return (selectedValues);
    }
}