----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 11:16:56 PM
-- Design Name: 
-- Module Name: full_adder_struct - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity full_adder_struct is
    Port ( Cin : in STD_LOGIC;
           A : in STD_LOGIC;
           B : in STD_LOGIC;
           S : out STD_LOGIC;
           Cout : out STD_LOGIC);
end full_adder_struct;

architecture Structural of full_adder_struct is

component xor2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC );
end component;

component xor3 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; C : in STD_LOGIC; Q : out STD_LOGIC );
end component;

component and2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC );
end component;

component or2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC );
end component;

signal AxorB, AandB, Temp : STD_LOGIC;

begin
    U1: xor3 port map (A => A, B => B, C => Cin, Q => S);
    U2: xor2 port map (A => A, B => B, Q => AxorB);
    U3: and2  port map (A => A, B => B, Q => AandB);
    U4: and2 port map (A => Cin, B => AxorB, Q => Temp);
    U5: or2 port map (A => AandB, B => Temp, Q => Cout);
end Structural;
